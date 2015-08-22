#include <SDL.h>
#include <stdlib.h>


#include "sdl.h"
#include "view.h"
#include "font.h"

Uint16 create_hicolor_pixel(SDL_PixelFormat *fmt, Uint8 red, Uint8 green, Uint8 blue) 
{
     Uint16 value;

     value = 
          ((red >> fmt->Rloss) << fmt->Rshift) +
          ((green >> fmt->Gloss) << fmt->Gshift) +
          ((blue >> fmt->Bloss) << fmt->Bshift);

     return value;
}

SDL_Surface *  sdl_init()
{
     SDL_Init(SDL_INIT_VIDEO);
     SDL_WM_SetCaption("G0-4T CONSOLE", "G0-4T CONSOLE");

     SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF);
     return screen;
}

void sdl_draw_text_buffer(text_buffer *text_buffer, SDL_Surface *screen)
{
     /* lock teh surface */
     SDL_LockSurface(screen);

     int x, y, i, j, q = 0;

     Uint16 pf = create_hicolor_pixel(screen->format, 255, 255, 255); 
     Uint16 pb = create_hicolor_pixel(screen->format, 0, 0, 128); 
     Uint16 *raw_pixels = (Uint16 *) screen->pixels;

     for (y = 0 ; y < text_buffer->h ; y++) {
          for (x = 0; x < text_buffer->w ; x++) {
               unsigned char c = text_buffer->buffer[x + (y * text_buffer->w)];
               unsigned char *p = &dos_font[c * 14];
               int sy = y * 14;
               int sx = x * 8;
               for (i = 0; i < 14 ; i++) {
                    for (j = 0 ; j < 8; j++) {
                         Uint16 col = (*p & (1 << j)) ? pf : pb;
                         int offset = ((screen->pitch / 2 * (sy + i)) + (sx + 8-j));
                         raw_pixels[offset] = col;
                    }
                    p++;
               }
          }
     }

     SDL_UnlockSurface(screen);
}


int sdl_main_loop(SDL_Surface *screen, goat_game *game)
{
     int keep_going = 1;
     SDL_Event event;
     unsigned char text_buffer[80 * 25];
     int i;

     for (i = 0 ; i < 80 * 25; i++) {
          text_buffer[i] = i % 255;
     }

     while (keep_going) {
          if (SDL_PollEvent(&event)) {
               switch(event.type) {
               case SDL_QUIT:
                    keep_going = 0;
                    break;
               case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                         keep_going = 0;
                         break;
                    case SDLK_DOWN:
                         game->p1->edit_ptr += 2;
                         break;
                    case SDLK_UP:
                         game->p1->edit_ptr -= 2;
                         break;
                    case SDLK_LEFT:
                         game->p1->edit_ptr -= 1;
                         break;
                    case SDLK_RIGHT:
                         game->p1->edit_ptr += 1;
                         break;
                    case SDLK_EQUALS:
                         goat_mem_inc(game->mem, game->p1->edit_ptr, 1);
                         break;
                    case SDLK_MINUS:
                         goat_mem_inc(game->mem, game->p1->edit_ptr, -1);
                    }
                    break;
               }
          }
          
          goat_player_view_refresh(game->p1_view);
          goat_player_view_refresh(game->p2_view);

          sdl_draw_text_buffer(game->text_buffer, screen);
          SDL_UpdateRect(screen, 0, 0, 0, 0);
          SDL_Flip(screen);
     }

     SDL_Quit();
     return 0;
}


