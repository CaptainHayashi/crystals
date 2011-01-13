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

/** @file    src/module.c
 *  @author  Michael Walker
 *  @brief   Functions to handle loading and unloading of the modules
 *           that make up the engine.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TESTSUITE
#include "tests/module.h"
#endif /* TESTSUITE */

#include "util.h"
#include "module.h"

module_set g_modules; /* The set of all modules in use */


/* This initialises the struct of modules to NULL and sets the load path */

bool_t
init_modules (const char *path)
{
  g_modules.path = malloc (sizeof (char) * (strlen (path) + 1));

  if (g_modules.path)
    {
      strncpy (g_modules.path, path, strlen (path) + 1);

      module_bare_init (&g_modules.gfx.metadata);
      module_bare_init (&g_modules.event.metadata);

      return SUCCESS;
    }
  else
    {
      error ("MODULE - init_modules - Couldn't allocate modules path.");
    }

  return FAILURE;
}


/* This does the bare minimum initialisation for a module */

void
module_bare_init (module_data *module)
{
  module->lib_handle = NULL;
  module->init       = NULL;
  module->term       = NULL;
}


/* This gets the path of a module, storing it in out */

bool_t
get_module_path (const char* module, const char* modulespath, char** out)
{
  char *path;

  path = calloc (strlen (modulespath) + strlen (module)
                 + strlen (MODULESUFFIX) + 1, sizeof (char));

  if (path)
    {
      strncpy (path, modulespath, strlen (modulespath));
      strncat (path, module, strlen (module));
      strncat (path, MODULESUFFIX, strlen (MODULESUFFIX));
    }
  else
    {
      error ("MODULE - get_module_path - couldn't allocate module path.");
      return FAILURE;
    }

  *out = path;
  return SUCCESS;
}


/* This finds the filename of a module and calls get_module */

bool_t
get_module_by_name (const char* name, const char *modulespath, module_data *module)
{
  bool_t out;        /* Soon-to-be return value of get_module */
  char *modulepath;  /* Buffer in which to store path to the module. */

  if (get_module_path (name, modulespath, &modulepath) == FAILURE)
    return FAILURE;


  /* And get the module */

  if (modulepath)
    {
      out = get_module (modulepath, module);
      free (modulepath);
      return out;
    }
  else
    {
      error ("MODULE - get_module_by_name - couldn't find module path.");
      return FAILURE;
    }
}


/* This opens a module file and runs any init code */

bool_t
get_module (const char* modulepath, module_data *module)
{

  if (module->lib_handle != NULL)
    return FAILURE;

  module->lib_handle = DLLOPEN(modulepath);

  if (module->lib_handle == NULL)
    {
      DLLERROR("get_module");
      return FAILURE;
    }


  /* Get init and termination functions if present */

  get_module_function (*module, "init", (mod_function_ptr*) &module->init);
  get_module_function (*module, "term", (mod_function_ptr*) &module->term);


  /* Execute init function if present */

  if (module->init != NULL)
    (*module->init) ();

  return SUCCESS;
}


#ifdef USE_LIBDL

/* Raise the last DLL-acquisition error message, if any. */

void
std_get_dll_error (const char function_name[])
{
  char *dlerr = dlerror ();

  if (dlerr != NULL)
    error ("MODULE - %s - Failed with dlerr: %s", function_name, dlerr);
}

#endif /* USE_LIBDL */


/* This loads a pointer to a function from a module */

bool_t
get_module_function (module_data module, const char *function, mod_function_ptr *func)
{
  /** char *dlerr; */

  *func = DLLLOOKUP(module.lib_handle, function);

  if (*func == NULL)
    {
      DLLERROR("get_module_function");
      return FAILURE;
    }

  return SUCCESS;
}


/* Load a graphics module. */

bool_t
load_module_gfx (const char* name, module_set* modules)
{
  if (get_module_by_name (name, modules->path, &modules->gfx.metadata) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "init_screen_internal",
                           (mod_function_ptr*)
                           &modules->gfx.init_screen_internal)
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "draw_rect_internal",
                           (mod_function_ptr*)
                           &modules->gfx.draw_rect_internal)
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "load_image_data",
                           (mod_function_ptr*)
                           &modules->gfx.load_image_data) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "free_image_data",
                           (mod_function_ptr*)
                           &modules->gfx.free_image_data) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "draw_image_internal",
                           (mod_function_ptr*)
                           &modules->gfx.draw_image_internal)
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "update_screen_internal",
                           (mod_function_ptr*)
                           &modules->gfx.update_screen_internal) 
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "scroll_screen_internal",
                           (mod_function_ptr*)
                           &modules->gfx.scroll_screen_internal)
      == FAILURE)
    return FAILURE;
  
  return SUCCESS;
}


/* Load an event module. */

bool_t
load_module_event (const char *name, module_set *modules)
{
  if (get_module_by_name (name, modules->path, &modules->event.metadata)
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->event.metadata,
                           "process_events_internal",
                           (mod_function_ptr*)
                           &modules->event.process_events_internal)
      == FAILURE)
    return FAILURE;

  if (get_module_function (modules->event.metadata,
                           "register_release_handle",
                           (mod_function_ptr*)
                           &modules->event.register_release_handle)
      == FAILURE)
    return FAILURE;

  return SUCCESS;
}


/* This closes an individual module and runs any termination code */

void
close_module (module_data *module)
{
  if (module->lib_handle)
    {
      if (module->term)
        {
          /* Call any termination code */
          (*module->term) ();
        }
      /* Close the lib handle */
      DLLCLOSE (module->lib_handle);
    }
}


/* This closes any loaded modules, run before program termination */

void
cleanup_modules (void)
{
  close_module (&g_modules.event.metadata);
  close_module (&g_modules.gfx.metadata);

  free (g_modules.path);
}
/* vim: set ts=2 sw=2 softtabstop=2: */
