#ifndef _GOAT_GAME_H
#define _GOAT_GAME_H

#include <SDL.h>
#include "mem.h"
#include "player.h"
#include "view.h"


#define GOAT_GAME_TICK_INTERVAL_MS 500
#define GOAT_GAME_PLAYER_TICK_INTERVAL 75

typedef struct goat_game goat_game;

struct goat_game {
     goat_player *p1, *p2;
     goat_player_view *p1_view, *p2_view;
     goat_mem *mem;
     text_buffer *text_buffer;
     unsigned int ctx;
     

     Uint32 tick_interval;
     Uint32 next_tick;
     
     Uint32 player_tick_interval;
     Uint32 player_next_tick;

};


goat_game * make_goat_game();
void goat_game_view_refresh(goat_game *game);
void goat_game_tick(goat_game *game);
void free_goat_game(goat_game *game);
void goat_game_handle_input(goat_game *game, SDL_Event *event);


#endif
