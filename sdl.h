#ifndef _GOAT_SDL_H
#define _GOAT_SDL_H

#include <SDL.h>

typedef struct text_buffer text_buffer;

struct text_buffer {
     unsigned int w, h;
     unsigned char *buffer;
};

typedef struct goat_sdl goat_sdl;

struct goat_sdl {
     SDL_Surface *screen;
     SDL_Joystick *p1_stick;
     SDL_Joystick *p2_stick;
};


goat_sdl * sdl_init();
text_buffer * make_text_buffer(int w, int h);
void free_text_buffer(text_buffer *text_buffer);
void text_buffer_write(text_buffer *text_buffer, int x, int y, char *str, size_t n);
void sdl_draw_text_buffer(text_buffer *text_buffer, SDL_Surface *screen);
void text_buffer_draw_box(text_buffer *text_buffer, int cx, int cy, int w, int h);


#endif
