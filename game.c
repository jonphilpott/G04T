#include <stdlib.h>
#include <assert.h>

#include "game.h"
#include "mem.h"
#include "player.h"
#include "view.h"
#include "thread.h"


goat_game * make_goat_game()
{
     goat_game *game = 
          (goat_game *) malloc(sizeof(goat_game));

     assert(game != NULL);

     goat_mem *mem = make_goat_mem(512);
     goat_player *p1 = make_goat_player(mem, 0, 64);
     goat_player *p2 = make_goat_player(mem, 128, 192);

     text_buffer *tb = make_text_buffer(80, 34);
     
     goat_player_view *p1v = make_goat_player_view(p1, 
                                                  p2,
                                                  mem,
                                                  0,
                                                  0,
                                                  tb);

     goat_player_view *p2v = make_goat_player_view(p2,
                                                  p1,
                                                  mem,
                                                  45,
                                                  0,
                                                  tb);
     game->p1 = p1;
     game->p2 = p2;
     game->text_buffer = tb;
     game->mem = mem;
     game->p1_view = p1v;
     game->p2_view = p2v;
         
     return game;
}


void free_goat_game(goat_game *game) 
{ 
     free_goat_player_view(game->p1_view);
     free_goat_player_view(game->p2_view);
     free_text_buffer(game->text_buffer);
     free_goat_player(game->p1);
     free_goat_player(game->p2);
     free_goat_mem(game->mem);
     free(game);
}
