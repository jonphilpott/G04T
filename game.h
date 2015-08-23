#ifndef _GOAT_GAME_H
#define _GOAT_GAME_H

#include "mem.h"
#include "player.h"
#include "view.h"


typedef struct goat_game goat_game;

struct goat_game {
     goat_player *p1, *p2;
     goat_player_view *p1_view, *p2_view;
     goat_mem *mem;
     text_buffer *text_buffer;
     unsigned int ctx;
};


goat_game * make_goat_game();
void goat_game_view_refresh(goat_game *game);
void goat_game_tick(goat_game *game);
void free_goat_game(goat_game *game);

#endif
