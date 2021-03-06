#include <SDL.h>
#include <stdlib.h>
#include <assert.h>


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

goat_sdl *  sdl_init()
{

     goat_sdl *gsdl = (goat_sdl *) malloc(sizeof(goat_sdl));
     assert(gsdl != NULL);

     SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
     SDL_WM_SetCaption("G0-4T CONSOLE", "G0-4T CONSOLE");

     SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF);
     assert(screen != NULL);
     gsdl->screen = screen;


     if (SDL_NumJoysticks() < 2) {
          printf("No game controllers detected. Quitting.");
          return NULL;
     }

     gsdl->p1_stick = SDL_JoystickOpen(0);
     gsdl->p2_stick = SDL_JoystickOpen(1);

     if (gsdl->p1_stick == NULL || gsdl->p2_stick == NULL) {
          return NULL;
     }

     printf ("joystick has %d buttons\n", 
             SDL_JoystickNumButtons(gsdl->p1_stick));

     printf ("joystick has %d axes\n", 
             SDL_JoystickNumAxes(gsdl->p1_stick));

     return gsdl;
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



text_buffer *
make_text_buffer(int w, int h) 
{
     text_buffer *tb = 
          (text_buffer *) malloc(sizeof(text_buffer));

     assert(tb != NULL);

     unsigned char *b = 
          (unsigned char *) malloc(w * h);

     bzero(b, w*h);

     assert(b != NULL);

     tb->w = w; 
     tb->h = h;
     tb->buffer = b;
     
     return tb;
}

void free_text_buffer(text_buffer *text_buffer)
{
     free(text_buffer->buffer);
     free(text_buffer);
}


void text_buffer_write(text_buffer *text_buffer, int x, int y, char *str, size_t n)
{
     unsigned int offset = (y * text_buffer->w) + x;
     memcpy(text_buffer->buffer + offset, str, n);
}


void text_buffer_draw_box(text_buffer *text_buffer, int cx, int cy, int w, int h)
{
     int x;
     int y;

     // top & bottom line
     for (x = cx+1 ; x < cx+w; x++) {
          text_buffer->buffer[(cy * text_buffer->w) + x] = '\xcd';
          text_buffer->buffer[((cy + h) * text_buffer->w) + x] = '\xcd';
     }

     // sides
     for (y = cy+1 ; y < cy+h; y++) {
          text_buffer->buffer[(y * text_buffer->w) + cx] = '\xba';
          text_buffer->buffer[(y * text_buffer->w) + cx + w] = '\xba';
     } 

     // corners
     text_buffer->buffer[(cy * text_buffer->w) + cx]           = '\xc9';
     text_buffer->buffer[(cy * text_buffer->w) + cx + w]       = '\xbb';
     text_buffer->buffer[((cy + h) * text_buffer->w) + cx]     = '\xc8';
     text_buffer->buffer[((cy + h) * text_buffer->w) + cx + w] = '\xbc';

}
