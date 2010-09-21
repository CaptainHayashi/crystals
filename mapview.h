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

/** @file    mapview.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for rendering functions.
 *
 *  The map renderer makes a distinction between the map data itself 
 *  and the additional data set tracking the map viewpoint.  Whereas 
 *  functions and structures related to the former are available in 
 *  map.c and map.h, the map viewpoint system is described in this 
 *  and mapview.c.
 */

#ifndef _MAPVIEW_H
#define _MAPVIEW_H

#include "map.h"

/* -- STRUCTURES -- */

/** A map viewpoint.
 *
 *  This contains data required to render a map, including the offset
 *  of the current viewpoint, which tiles to render on the next
 *  render pass, and suchlike.
 */

struct map_view
{
  int x_offset;               /**< Offset of the left edge of the
                                 screen, in pixels from the left edge
                                 of the map.  Can be negative. */

  int y_offset;               /**< Offset of the top edge of the
                                 screen, in pixels from the top edge
                                 of the map. Can be negative.*/

  struct map *map;            /**< Pointer to the map being viewed. */

  unsigned char *dirty_tiles; /**< Matrix of "dirty" tiles, or tiles
                                 to be re-rendered on the next
                                 rendering pass. 

                                 @note  To set a tile as dirty, index
                                 [tile x + (tile y * map width)
                                 should be set to the number of
                                 layers in the map.  This is because
                                 of the way the render code works.
                                 Eventually there will be a function
                                 for setting tiles to dirty without
                                 needing to pay attention to this. */
};

/* -- GLOBAL VARIABLES -- */

extern const char FN_TILESET[]; /**< Tileset filename. */

/* -- PROTOTYPES -- */

/** Initialise a map view.
 *
 *  @param map  Pointer to the map to associate with the map view.
 *
 *  @return  a pointer to the map view, or NULL for allocation
 *  failure.
 */

struct map_view *
init_mapview (struct map *map);

/** Render the dirty tiles on a map.
 *
 *  @param mapview  Pointer to the map view to render.
 */

void
render_map (struct map_view *mapview);

/** Render a given layer on a map.
 *
 *  @param mapview  Pointer to the map view to render.
 *
 *  @param layer    The layer to render.
 */

void
render_map_layer (struct map_view *mapview, unsigned char layer);

/** Scroll the map on-screen, re-rendering it in its new position.
 *
 *  @param mapview    The map view to render.
 *
 *  @param direction  The cardinal direction (NORTH, SOUTH, EAST or
 *                    WEST) to scroll in.
 */

void
scroll_map (struct map_view *mapview, int direction);

/** Mark a rectangle of tiles as being dirty.
 *
 *  The X and Y tile offsets are absolute (that is, offset from 
 *  tile (0, 0) rather than the tile currently shown at the top-left 
 *  of the screen.
 *
 *  @param mapview  Pointer to the map view to render.
 *
 *  @param start_x  The X co-ordinate of the start of the rectangle,
 *                  as a tile offset from the left edge of the map.
 *
 *  @param start_y  The Y co-ordinate of the start of the rectangle, 
 *                  as a tile offset from the top edge of the map.
 *
 *  @param width    Width of the tile rectangle, in tiles.
 *
 *  @param height   Height of the tile rectangle, in tiles.
 */

void
mark_dirty_rect (struct map_view *mapview,
                 int start_x, int start_y, int width, int height);

/** De-initialise a mapview.
 *
 *  @warning This frees the mapview structure, but NOT the map
 *  connected to it.  Use cleanup_map to remove the map.
 *
 *  @param mapview  Pointer to the map view to de-allocate.
 */

void
cleanup_mapview (struct map_view *mapview);

#endif /* _MAPVIEW_H */
