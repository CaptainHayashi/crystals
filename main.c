#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "util.h"
#include "object.h"
#include "parser.h"
#include "module.h"
#include "events.h"
#include "graphics.h"
#include "bindings.h"
#include "mapview.h"
#include "map.h"

struct map *g_map;
struct map_view *g_mapview;

int g_running;

int
main (int argc, char **argv)
{
  /* Placeholder for command line stuff. */
  argc = argc;
  argv = argv;

  if (init () == SUCCESS)
    {
      g_running = TRUE;
      main_loop ();
    }
  cleanup ();
  return 0;
}

int
init (void)
{
  char *module_path;

  init_parser ();

  if (config_parse_file ("config/default.cfg") == SUCCESS)
    {
      module_path = config_get_value ("module_path");
    }
  else
    {
      module_path = DEFMODPATH;
    }

  if (init_modules (module_path) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init modules!\n");
      return FAILURE;
    }

  if (init_graphics () == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init gfx!\n");
      return FAILURE;
    }

  if (init_objects () == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init objects!\n");
      return FAILURE;
    }

  g_map = init_test_map ();

  if (g_map == NULL)
    {
      fprintf (stderr, "ERROR: Map did not init!\n");
      return FAILURE;
    }

  g_mapview = init_mapview (g_map);

  if (g_mapview == NULL)
    {
      fprintf (stderr, "ERROR: Map view did not init!\n");
      return FAILURE;
    }

  init_bindings ();
  run_file ("tests/lua.lua");
  init_events ();

  return SUCCESS;
}


void
main_loop (void)
{
  render_map (g_mapview);

  while (g_running)
    {
      (*g_modules.gfx.update_screen) ();
      (*g_modules.event.process_events) ();
      handle_held_keys ();
    }
}

void
cleanup (void)
{
  cleanup_mapview (g_mapview);
  cleanup_map (g_map);
  cleanup_objects ();
  cleanup_events ();
  cleanup_graphics ();
  cleanup_bindings ();
  cleanup_modules ();
  cleanup_parser ();
}

/* vim: set ts=2 sw=2 softtabstop=2: */
