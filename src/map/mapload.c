/*
 * Crystals (working title)
 *
 * Copyright (c) 2010, 2011
 *               Matt Windsor, Michael Walker
 *               and Alexander Preisinger.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   * The names of contributors may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * AFOREMENTIONED COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    src/field/mapload.c
 * @author  Matt Windsor
 * @brief   Map loader.
 */

#include "../crystals.h"


/* -- CONSTANTS -- */

static const uint16_t MAP_VERSION = 1;	 /**< Expected map version. */
static const long CHUNK_NOT_FOUND = -1;	/**< Sentinel chunk position. */

enum chunk_ids
{
  ID_FORM,
  ID_HEADER,
  ID_VERSION,
  ID_DIMENSIONS,
  ID_TAGS,
  ID_VALUES,
  ID_ZONES,
  ID_PROPERTIES,
  NUM_CHUNKS
};


/**
 * Length in bytes of each chunk ID.
 */
static const size_t ID_LENGTH = 4;


/**
 * Array of ChunkID identifiers.
 */
static const char CHUNK_IDS[NUM_CHUNKS][5] = {
  "FORM",			/* ID_FORM */
  "CMFT",			/* ID_HEADER */
  "VERS",			/* ID_VERSION */
  "DIMS",			/* ID_DIMENSIONS */
  "TAGS",			/* ID_TAGS */
  "VALS",			/* ID_VALUES */
  "ZONE",			/* ID_ZONES */
  "PROP",			/* ID_PROPERTIES */
};


/* -- STATIC DECLARATIONS -- */

/**
 * Parses the given file as a map file and attempts to return a map
 * created from its contents.
 *
 * @param file  The file to read from.
 *
 * @return  A pointer to a map created from the given map file, or
 *          NULL if there were errors during the parsing.
 */
static map_t *parse_map_file (FILE *file);


/**
 * Finds the locations of the chunks in the map file that the loader
 * is interested in.
 *
 * @param file  The file to read from.  This should be seeked to the
 *              start of the file.
 *
 * @return  An array of long integers, parallel to the CHUNK_IDS
 *          array, containing the positions of chunks.  A -1 in any
 *          index denotes an error in finding the chunk.
 */
static long *find_chunks (FILE *file);


/**
 * Initialises an array to hold the chunk position set.
 *
 * @return  an array suitable for holding the chunk positions, or
 *          NULL if allocation failed.
 */
static long *init_chunk_positions_array (void);


/**
 * Scans the main body of the file for chunk positions.
 *
 * @param file             The file to read from.  This should be
 *                         seeked to the start of the body (that is,
 *                         after the FORM header).
 * @param file_length      The length of the body of the file, in
 *                         bytes.  This is used for consistency
 *                         checking.
 * @param chunk_positions  The array to populate with the chunk
 *                         positions.
 */
static void scan_body_for_chunks (FILE *file, uint32_t file_length,
				  long *chunk_positions);


/**
 * Checks the required chunks to see if any are missing.
 *
 * @param chunk_positions  The array of chunk positions, as returned
 *                         by find_chunks.
 *
 * @return TRUE if one or more chunks is missing; FALSE otherwise.
 */
static bool chunks_missing (long *chunk_locations);


/**
 * Skips to the given chunk position in a file.
 *
 * @param file            The file to seek within.
 * @param chunk_position  The position within the file, in bytes from
 *                        the file start, of the chunk.
 */
static void skip_to_chunk (FILE *file, long chunk_position);


/**
 * Reads the version number chunk from the file.
 * @param file            The file to read from.
 * @param chunk_position  The position within the file, in bytes from
 *                        the file start, of the chunk.
 *
 * @return  the version number.
 */
static uint16_t read_map_version_chunk (FILE *file,
					long chunk_position);


/**
 * Reads the version number from the file.
 *
 * @param file  The file to read from.
 *
 * @return  the version number.
 */
static uint16_t read_map_version (FILE *file);

/**
 * Reads the map dimensions chunk from the file, initialising the map
 * using the results.
 *
 * @param file                 The file to read from.
 * @param chunk_position       The position within the file, in bytes
 *                             from the file start, of the chunk.
 * @param out_width            Pointer to a variable in which the
 *                             width of the map, in tiles, is to be
 *                             stored.
 * @param out_height           Pointer to a variable in which the
 *                             height of the map, in tiles, is to be
 *                             stored.
 * @param out_max_layer_index  Pointer to a variable in which the
 *                             maximum layer index is to be stored.
 * @param out_max_zone_index   Pointer to a variable in which the
 *                             maximum zone index is to be stored.
 */
static void read_map_dimensions_chunk (FILE *file,
				       long chunk_position,
				       dimension_t *out_width,
				       dimension_t *out_height,
				       layer_index_t
				       *out_max_layer_index,
				       zone_index_t
				       *out_max_zone_index);


/**
 * Reads the map dimensions from the file, initialising the map using
 * the results.
 *
 * @param file                 The file pointer to read from.
 * @param out_width            Pointer to a variable in which the
 *                             width of the map, in tiles, is to be
 *                             stored.
 * @param out_height           Pointer to a variable in which the
 *                             height of the map, in tiles, is to be
 *                             stored.
 * @param out_max_layer_index  Pointer to a variable in which the
 *                             maximum layer index is to be stored.
 * @param out_max_zone_index   Pointer to a variable in which the
 *                             maximum zone index is to be stored.
 */
static void read_map_dimensions (FILE *file,
				 dimension_t *out_width,
				 dimension_t *out_height,
				 layer_index_t *out_max_layer_index,
				 zone_index_t *out_max_zone_index);


/**
 * Reads the map layer tags chunk from a file.
 *
 * @param file            The file to read from.
 * @param map             The map to populate with the read data.
 * @param chunk_position  The position within the file, in bytes from
 *                        the file start, of the chunk.
 */
static void read_map_tags_chunk (FILE *file, map_t *map,
				 long chunk_position);


/**
 * Reads the map layer tags from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 */
static void read_map_tags (FILE *file, map_t *map);


/**
 * Reads the map value planes chunk from a file.
 *
 * @param file            The file to read from.
 * @param map             The map to populate with the read data.
 * @param chunk_position  The position within the file, in bytes from
 *                        the file start, of the chunk.
 */
static void read_map_value_planes_chunk (FILE *file,
					 map_t *map,
					 long chunk_position);


/**
 * Reads the map value planes from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 */
static void read_map_value_planes (FILE *file, map_t *map);


/**
 * Reads a map value plane from a file.
 *
 * Each plane corresponds to one layer of the map.
 *
 * @param file   The file pointer to read from.
 * @param map    The map to populate with the read data.
 * @param layer  The index of the layer being read.
 */
static void read_map_value_plane (FILE *file, map_t *map,
				  layer_index_t layer);


/**
 * Reads the map zone planes chunk from a file.
 *
 * @param file            The file pointer to read from.
 * @param map             The map to populate with the read data.
 * @param chunk_position  The position within the file, in bytes from
 *                        the file start, of the chunk.
 */
static void read_map_zone_planes_chunk (FILE *file,
					map_t *map,
					long chunk_position);


/**
 * Reads the map zone planes from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 */
static void read_map_zone_planes (FILE *file, map_t *map);


/**
 * Read layer zone data from a file.
 *
 * @param file   The file pointer to read from.
 * @param map    The map to populate with the read data.
 * @param layer  The index of the layer being read.
 */
static void read_layer_zone_plane (FILE *file, map_t *map,
				   layer_index_t layer);


/**
 * Read the zone properties chunk from a file.
 *
 * @param file            The file pointer to read from.
 * @param map             The map to populate with the read data.
 * @param chunk_position  The position within the file, in bytes from
 *                        the file start, of the chunk.
 */
static void read_map_zone_properties_chunk (FILE *file,
					    map_t *map,
					    long chunk_position);


/**
 * Read the properties of each zone in the map from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 */
static void read_map_zone_properties (FILE *file, map_t *map);


/**
 * Check that a magic sequence is present.
 *
 * @param file      The file pointer to read from.
 * @param sequence  The byte sequence (one of the chunk IDs).
 *
 * @return          TRUE if it is present, FALSE otherwise.
 */
static bool check_magic_sequence (FILE *file, const char sequence[]);


/* -- DEFINITIONS -- */

/* Reads a map from a file using the Crystals map format. */
map_t *
load_map (const char path[])
{
  int close_result;
  map_t *map;
  FILE *file = fopen (path, "rb");

  g_assert (file != NULL);

  map = parse_map_file (file);

  close_result = fclose (file);
  g_assert (close_result == 0);

  return map;
}


/* Parses the given file as a map file and attempts to return a map
 * created from its contents.
 */
static map_t *
parse_map_file (FILE *file)
{
  long *chunks = find_chunks (file);
  dimension_t new_map_width;
  dimension_t new_map_height;
  layer_index_t new_max_layer_index;
  zone_index_t new_max_zone_index;
  map_t *map;

  if (chunks_missing (chunks))
    {
      fatal ("MAPLOAD - parse_map_file - Missing required chunks.");
    }
  else if (read_map_version_chunk (file, chunks[ID_VERSION])
	   != MAP_VERSION)
    {
      fatal ("MAPLOAD - parse_map_file - Incorrect version.");
    }

  read_map_dimensions_chunk (file, chunks[ID_DIMENSIONS],
			     &new_map_width, &new_map_height,
			     &new_max_layer_index,
			     &new_max_zone_index);

  map =
    init_map (new_map_width, new_map_height, new_max_layer_index,
	      new_max_zone_index);

  read_map_tags_chunk (file, map, chunks[ID_TAGS]);
  read_map_value_planes_chunk (file, map, chunks[ID_VALUES]);
  read_map_zone_planes_chunk (file, map, chunks[ID_ZONES]);
  read_map_zone_properties_chunk (file, map, chunks[ID_PROPERTIES]);

  free (chunks);

  return map;
}


/* Finds the locations of the chunks in the map file that the loader
 * is interested in.
 */
static long *
find_chunks (FILE *file)
{
  uint32_t file_length;
  int seek_result;
  long *chunk_positions;

  g_assert (file);

  chunk_positions = init_chunk_positions_array ();
  if (chunk_positions == NULL)
    {
      fatal ("MAPLOAD - find_chunks - Could not allocate memory.");
    }

  seek_result = fseek (file, 0, SEEK_SET);
  g_assert (seek_result == 0);

  /* We expect the file to start with "FORM", as it should be an
   * IFF containing only one CMFT file.
   */
  if (!(check_magic_sequence (file, CHUNK_IDS[ID_FORM])))
    {
      fatal ("MAPLOAD - find_chunks - Missing form header.");
    }
  file_length = read_uint32 (file);

  /* The type of file, which is expected here, should be CMFT. */
  if (!(check_magic_sequence (file, CHUNK_IDS[ID_HEADER])))
    {
      fatal ("MAPLOAD - find_chunks - Incorrect file type.");
    }

  scan_body_for_chunks (file, file_length, chunk_positions);
  return chunk_positions;
}


/* Initialises an array to hold the chunk position set. */
static long *
init_chunk_positions_array (void)
{
  long *result = xcalloc (NUM_CHUNKS, sizeof (long));
  if (result)
    {
      int i;
      for (i = ID_VERSION; i < NUM_CHUNKS; i += 1)
	{
	  result[i] = CHUNK_NOT_FOUND;
	}

      /* These two are in the same place in all well-formed maps. */
      result[ID_FORM] = 0;
      result[ID_HEADER] = 4;
    }
  return result;
}


/* Scans the main body of the file for chunk positions. */
static void
scan_body_for_chunks (FILE *file,
		      uint32_t file_length, long *chunk_positions)
{
  int i;
  int seek_result;
  uint32_t chunk_length;
  size_t num_bytes;
  char *chunk_name = xcalloc (ID_LENGTH + 1, sizeof (char));

  while (feof (file) == 0)
    {
      num_bytes = fread (chunk_name, sizeof (char), ID_LENGTH, file);
      if (num_bytes != ID_LENGTH)
	{
	  break;
	}

      chunk_length = read_uint32 (file);

      for (i = 0; i < NUM_CHUNKS; i++)
	{
	  if (strcmp (chunk_name, CHUNK_IDS[i]) == 0)
	    {
	      g_debug ("Found %s chunk in map at position %lx",
		       chunk_name, ftell (file));
	      chunk_positions[i] = ftell (file);
	    }
	}
      seek_result = fseek (file, (long) chunk_length, SEEK_CUR);
      g_assert (seek_result == 0);
    }

  /* Length of file body + FORM chunk ID + size mark */
  if (ftell (file) != (long) (file_length
			      + ID_LENGTH + sizeof (uint32_t)))
    {
      error ("MAPLOAD - find_chunks - Size mismatch. %lx %lx",
	     file_length + ID_LENGTH + sizeof (uint32_t),
	     ftell (file));
    }

  free (chunk_name);
}


/* Checks the required chunks to see if any are missing. */
static bool
chunks_missing (long *chunk_positions)
{
  int i;

  for (i = 0; i < NUM_CHUNKS; i += 1)
    {
      if (chunk_positions[i] == CHUNK_NOT_FOUND)
	return true;
    }
  return false;
}


/* Skips to the given chunk position in a file. */
static void
skip_to_chunk (FILE *file, long chunk_position)
{
  int seek_result;

  g_assert (chunk_position >= 0);

  g_debug ("Skipping to chunk at %lx...", chunk_position);
  seek_result = fseek (file, chunk_position, SEEK_SET);
  g_assert (seek_result == 0);
}


/* Reads the version number chunk from the file. */
static uint16_t
read_map_version_chunk (FILE *file, long chunk_position)
{
  skip_to_chunk (file, chunk_position);
  return read_map_version (file);
}


/* Reads the version number from the file. */
static uint16_t
read_map_version (FILE *file)
{
  return read_uint16 (file);
}


/* Reads the map dimensions chunk from the file. */
static void
read_map_dimensions_chunk (FILE *file,
			   long chunk_position,
			   dimension_t *out_width,
			   dimension_t *out_height,
			   layer_index_t *out_max_layer_index,
			   zone_index_t *out_max_zone_index)
{
  skip_to_chunk (file, chunk_position);
  read_map_dimensions (file, out_width, out_height,
		       out_max_layer_index, out_max_zone_index);
}


/* Reads the map dimensions from the file. */
static void
read_map_dimensions (FILE *file,
		     dimension_t *out_width,
		     dimension_t *out_height,
		     layer_index_t *out_max_layer_index,
		     zone_index_t *out_max_zone_index)
{
  g_assert (out_width != NULL);
  g_assert (out_height != NULL);
  g_assert (out_max_layer_index != NULL);
  g_assert (out_max_zone_index != NULL);

  *out_width = read_uint16 (file);	/* In tiles */
  *out_height = read_uint16 (file);	/* In tiles */
  *out_max_layer_index = read_uint16 (file);
  *out_max_zone_index = read_uint16 (file);
}


/* Reads the map layer tags chunk from a file. */
void
read_map_tags_chunk (FILE *file, map_t *map, long chunk_position)
{
  skip_to_chunk (file, chunk_position);
  read_map_tags (file, map);
}


/* Reads the map layer tags from a file. */
static void
read_map_tags (FILE *file, map_t *map)
{
  layer_index_t l;
  for (l = 0; l <= get_max_layer (map); l++)
    {
      set_layer_tag (map, l, read_uint16 (file));
    }
}


/* Reads the map value planes chunk from a file. */
static void
read_map_value_planes_chunk (FILE *file,
			     map_t *map, long chunk_position)
{
  skip_to_chunk (file, chunk_position);
  read_map_value_planes (file, map);
}


/* Reads the map value planes from a file. */
static void
read_map_value_planes (FILE *file, map_t *map)
{
  layer_index_t l;

  for (l = 0; l <= get_max_layer (map); l += 1)
    {
      read_map_value_plane (file, map, l);
    }
}


/* Reads a map value plane from a file. */
static void
read_map_value_plane (FILE *file, map_t *map, layer_index_t layer)
{
  dimension_t x;
  dimension_t y;

  for (x = 0; x < get_map_width (map); x += 1)
    {
      for (y = 0; y < get_map_height (map); y += 1)
	{
	  set_tile_value (map, layer, x, y, read_uint16 (file));
	}
    }
}


/* Reads the map zone plane chunk from a file. */
static void
read_map_zone_planes_chunk (FILE *file,
			    map_t *map, long chunk_position)
{
  skip_to_chunk (file, chunk_position);
  read_map_zone_planes (file, map);
}


/* Reads the map zone data from a file. */
static void
read_map_zone_planes (FILE *file, map_t *map)
{
  layer_index_t l;

  for (l = 0; l <= get_max_layer (map); l += 1)
    {
      read_layer_zone_plane (file, map, l);
    }
}


/* Reads layer zone data from a file. */
static void
read_layer_zone_plane (FILE *file, map_t *map, layer_index_t layer)
{
  dimension_t x;
  dimension_t y;

  for (x = 0; x < get_map_width (map); x++)
    {
      for (y = 0; y < get_map_height (map); y++)
	{
	  set_tile_zone (map, layer, x, y, read_uint16 (file));
	}
    }
}


/* Reads the zone properties chunk from a file. */
static void
read_map_zone_properties_chunk (FILE *file,
				map_t *map, long chunk_position)
{
  skip_to_chunk (file, chunk_position);
  read_map_zone_properties (file, map);
}


/* Reads the zone properties from a file. */
static void
read_map_zone_properties (FILE *file, map_t *map)
{
  zone_index_t i;
  for (i = 0; i <= get_max_zone (map); i += 1)
    {
      set_zone_properties (map, i, read_uint16 (file));
    }
}


/* Checks that a magic sequence is present. */
static bool
check_magic_sequence (FILE *file, const char sequence[])
{
  char *check = xcalloc (strlen (sequence) + 1, sizeof (char));
  size_t count =
    fread (check, sizeof (char), strlen (sequence), file);

  g_assert (count == strlen (sequence));

  if (strcmp (sequence, check) != 0)
    {
      error ("MAPLOAD - check_magic_sequence - Expected  %s; got %s",
	     sequence, check);
      free (check);
      return false;
    }

  free (check);
  return true;
}
