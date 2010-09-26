/*
 * Crystals (working title) 
 *
 * Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
 *                    Preisinger.
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

/** @file    object.c
 *  @author  Matt Windsor
 *  @brief   Low-level object functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "object.h"
#include "main.h"
#include "util.h"

struct object_t *g_objects[HASH_VALS];

int
init_objects (void)
{
  unsigned int i;

  for (i = 0; i < HASH_VALS; i++)
    g_objects[i] = NULL;

  /* TEST DATA */

  {
    struct object_t *test;
    struct object_t *test2;

    test = add_object ("Test1", "null");
    test2 = add_object ("Test2", "null");
    
    set_object_tag (test, 1);
    set_object_tag (test2, 1);
    set_object_image (test, "testobj.png", 0, 0, 60, 60, 16, 48);
    set_object_image (test2, "testobj.png", 16, 0, 70, 70, 16, 48);
  }

  return SUCCESS;
}

struct object_t *
add_object (const char object_name[],
            const char script_filename[])
{
  struct object_t *object;
  struct object_t *result;

  /* Sanity-check passed strings. */

  if (object_name == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Object name is NULL.\n");
      return FAILURE;
    }

  if (script_filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Script filename is NULL.\n");
      return FAILURE;
    }

  /* Try to allocate an object. */

  object = malloc (sizeof (struct object_t));

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Allocation failed for %s.\n", 
               object_name);
      return NULL;
    }

  /* Try to allocate and initialise an object image. */

  object->image = malloc (sizeof (struct object_image));

  if (object->image == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Allocation failed for rnode of %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  /* Try to copy the object name over. */

  object->name = malloc (sizeof (char) * (strlen (object_name) + 1));

  if (object->name == NULL)
    {
      fprintf (stderr, "OBJECT: Error: ON allocation failed for %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  strncpy (object->name, object_name,
           sizeof (char) * strlen (object_name) + 1);

  /* Try to copy the filename over. */

  object->script_filename = malloc (sizeof (char)
                                    * (strlen (script_filename) + 1));

  if (object->script_filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: FN allocation failed for %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  /* Finally, nullify everything else. */

  object->tag = 0;
  object->is_dirty = 0;
  object->next = NULL;

  init_object_image (object->image, object);

  /* Try to store the object. */

  result = get_object (object_name, object);

  if (result == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Store failed for %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  return result;
}

int
set_object_tag (struct object_t *object, 
                layer_t tag)
{
  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set tag of null object.\n");
      return FAILURE;
    }

  object->tag = tag;
  
  return SUCCESS;
}

int
set_object_image (struct object_t *object, 
                  const char filename[],
                  short image_x,
                  short image_y,
                  unsigned int map_x,
                  unsigned int map_y,
                  unsigned short width,
                  unsigned short height)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set image of null object.\n");
      return FAILURE;
    }

  if (object->image == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Object %s has no image dataset.\n", 
               object->name);
      return FAILURE;
    }

  if (filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set image FN to null.\n");
      return FAILURE;
    }

  /* Set up the filename. */

  if (object->image->filename != NULL)
    free (object->image->filename);

  object->image->filename = malloc (sizeof (char) * (strlen (filename) + 1));

  if (object->image->filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Couldn't alloc image FN for %s.\n",
               object->name);
      return FAILURE;
    }

  strncpy (object->image->filename, filename, (strlen (filename) + 1));

  /* Copy everything else. */

  object->image->image_x = image_x;
  object->image->image_y = image_y;
  object->image->map_x = map_x;
  object->image->map_y = map_y;
  object->image->width = width;
  object->image->height = height;

  return SUCCESS;
}

int
set_object_coordinates (struct object_t *object, 
                        unsigned int x, 
                        unsigned int y,
                        unsigned short reference)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set coords on NULL object.\n");
      return FAILURE;
    }

  if (object->image == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Object %s has no image dataset.\n", 
               object->name);
      return FAILURE;
    }

  /* Set the coordinates. */

  object->image->map_x = x;
  object->image->map_y = y;

  if (reference == BOTTOM_LEFT)
    {
      /* Check to see if the offset will send the object off the map. */

      if (object->image->map_y < object->image->height - 1)
        {
          fprintf (stderr, "OBJECT: Error: Object %s has bad coords.\n", 
                   object->name);
          return FAILURE;
        }

      object->image->map_y -= (object->image->height - 1);
    }

  return SUCCESS;
}

int
set_object_dirty (struct object_t *object, 
                  struct map_view *mapview)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set a NULL object dirty.\n");
      return FAILURE;
    }

  if (mapview == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried dirtying on a NULL mapview.\n");
      return FAILURE;
    }

  /* If we're already dirty, no need to run this again. */

  if (object->is_dirty == TRUE)
    return SUCCESS;

  /* Ensure the object's co-ordinates don't go over the map
     width/height! */

  if ((object->image->map_x + object->image->width
       > mapview->map->width * TILE_W)
      || (object->image->map_y + object->image->height 
          > mapview->map->height * TILE_H))
    {
      fprintf (stderr, "OBJECT: Error: Object %s out of bounds.\n", 
               object->name);
      return FAILURE;
    }

  /* And now, the business end. */

  if (object->tag != 0)
    {
      object->is_dirty = TRUE;
      return add_object_image (mapview, object->tag, object->image);
    }

  return FAILURE;
}

void
free_object (struct object_t *object)
{
  if (object)
    {
      if (object->name)
        free (object->name);

      if (object->script_filename)
        free (object->script_filename);

      if (object->image)
        free_object_image (object->image);

      free (object);
    }
}

int
delete_object (const char object_name[])
{
  int h;
  struct object_t *object, *prev;

  h = ascii_hash (object_name);
  prev = NULL;

  /* Iterate through the hash bucket to find the correct object, then 
     delete its data and node. */
  for (object = g_objects[h]; object != NULL; object = prev->next)
    {
      if (strcmp (object_name, object->name) == 0)
        {
          if (prev == NULL)
            g_objects[h] = object->next;
          else
            prev->next = object->next;
          
          free_object (object);
          return 1;
      }
    }
  return 0;
}

/* Delete all objects. */
void
clear_objects (void)
{
  int i;
  struct object_t *p, *next;
  
  for (i = 0; i < HASH_VALS; i++)
    {
      for (p = g_objects[i]; p != NULL; p = next)
        {
          next = p->next;
          /* Delete the object data and node */
          free_object (p);
        }
      g_objects[i] = NULL;
  }
}

struct object_t *
get_object (const char object_name[], struct object_t *add_pointer)
{
  int h; 
  struct object_t *object;

  /* Get the hash of the object's filename so we can search in the correct 
     bucket. */
  h = ascii_hash (object_name);

  /* Now try to find the object. */
  for (object = g_objects[h]; object != NULL; object = object->next)
    {
      if (strcmp (object_name, object->name) == 0)
        {
          /* If there is a pointer to add, and there's already a node
             with this object name, then raise an error.  Names
             should be unique. */

          if (add_pointer != NULL)
            {
              fprintf (stderr, "OBJECT: Error: Duplicate object %s!\n", 
                       object_name);

              free_object (add_pointer);
              return NULL;
            }

          return object;
        }
    }

  /* If we are given a pointer to add, and the object doesn't already 
     exist, then add the object to the start of the linked list. */
  if (add_pointer)
    {
      add_pointer->next = g_objects[h];
      g_objects[h] = add_pointer;

      return g_objects[h];
    }

  /* Return NULL, if all else fails. */
  return NULL;
} 
            
void
cleanup_objects (void)
{
  clear_objects ();
}
