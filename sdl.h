#ifndef _GOAT_SDL_H
#define _GOAT_SDL_H

#include <SDL.h>
#include "player.h"
#include "game.h"



SDL_Surface * sdl_init();
int sdl_main_loop(SDL_Surface *screen, goat_game *game);
void sdl_draw_text_buffer(text_buffer *text_buffer, SDL_Surface *screen);


#endif
