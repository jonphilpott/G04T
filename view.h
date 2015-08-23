#ifndef _GOAT_VIEW_H
#define _GOAT_VIEW_H

#include "player.h"
#include "mem.h"
#include "game.h"


typedef struct goat_player_view goat_player_view;
typedef struct text_buffer text_buffer;

struct text_buffer {
     unsigned int w, h;
     unsigned char *buffer;
};

struct goat_player_view {
     goat_player *player;
     goat_player *opponent;
     goat_mem    *mem;

     /* start corner, end corner */
     unsigned int cx, cy;

     text_buffer *text_buffer;
};




text_buffer * make_text_buffer(int w, int h);
void free_text_buffer(text_buffer *text_buffer);

void text_buffer_write(text_buffer *text_buffer, int x, int y, char *str, size_t n);
unsigned int goat_player_has_thread_here(goat_player *player, unsigned int loc);
goat_player_view * make_goat_player_view(goat_player *player, 
                                         goat_player *opponent, 
                                         goat_mem *mem,
                                         unsigned int cx,
                                         unsigned int cy,
                                         text_buffer *text_buffer
     );

void goat_player_view_refresh(goat_player_view *view);
void free_goat_player_view(goat_player_view *view);

#endif

