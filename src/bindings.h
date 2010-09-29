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

/** @file   src/bindings.h
 *  @author Alexander Preisinger
 *  @brief  Prototypes and declarations for generic language bindings.
 */

#ifndef _BINDINGS_H
#define _BINDINGS_H

#include "mapview.h"

/* -- STRUCTURES -- */

/** Game Data struct
 *
 * Contains all information that will be passed to the bindings
 * for scripting purpose.
 *
 * @todo don't forget to add more here and asking hayashi what else to add.
 */

struct game_data
{
  struct map_view *map_view; /**< Pointer to the mapview */
};

/* -- GLOBAL VARIABLES -- */

extern struct game_data g_game_data; /**< global variable for the game_data
                                          struct */

/* -- PROTOTYPES -- */

/** Init language bindings
 *
 *  @return Return SUCCESS if success else FAILURE
 */


int
init_bindings (void);


/** Cleanup language bindings */

void
cleanup_bindings(void);


/** Executes the given file.
 *
 *  @param path   Path to the file.
 *
 *  @return SUCCESS for success, FAILURE for failure.
 */

int
run_file (const char *path);

#endif /* _BINDINGS_H */

/* vim: set ts=2 sw=2 softtabstop=2: */
