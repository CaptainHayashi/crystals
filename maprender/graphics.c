#include <SDL.h>
#include <SDL/SDL_image.h>

#include "graphics.h"

static SDL_Surface *s_screen;

int
init_graphics (void)
{
  if (SDL_Init (SDL_INIT_VIDEO) == 0)
    {
      s_screen = SDL_SetVideoMode (SCREEN_W,
                                   SCREEN_H,
                                   SCREEN_D,
                                   SDL_SWSURFACE);

      if (s_screen)
        {
          return 1;
        }
      else
        {
          fprintf (stderr, "ERROR: Couldn't get screen!\n");
          cleanup_graphics ();
        }
    } 
  else 
    {
      fprintf (stderr, "ERROR: Could not init SDL!\n");
    }

  return 0;
}

void
draw_image (const char filename[], 
            unsigned int image_x,
            unsigned int image_y, 
            unsigned int screen_x, 
            unsigned int screen_y,
            unsigned int width, 
            unsigned int height)
{
  SDL_Surface *image;

  image = IMG_Load(filename);

  if (image)
    {
      SDL_Rect source, dest;
      
      source.x = image_x;
      source.y = image_y;
      dest.x = screen_x;
      dest.y = screen_y;

      source.w = dest.w = width;
      source.h = dest.h = height;

      SDL_BlitSurface (image, &source, s_screen, &dest); 
    }

  SDL_FreeSurface (image);
}

void
update_screen (void)
{
  SDL_Flip (s_screen);
  SDL_Delay (30);
}

void
cleanup_graphics (void)
{
  SDL_Quit ();
}

