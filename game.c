#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sdl.h"
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
     game->ctx = 1;
         
     return game;
}

void goat_game_view_refresh(goat_game *game) 
{
     char *static_lines[] = { 
          " CYBERGOAT ",
          "  SYSTEMS  ",
          " SYS:G0-4T ",
          "-----------"
     };
     int i;

     for (i = 0 ; i < 3 ; i++) {
          text_buffer_write(game->text_buffer, 
                            34, 
                            i+1,
                            static_lines[i],
                            strlen(static_lines[i])
               );
     }
}

void goat_game_tick(goat_game *game)
{
     goat_player *player = (game->ctx & 1) ? game->p1 : game->p2;
     goat_player_tick(player);
     game->ctx++;
}


void goat_game_handle_input(goat_game *game, SDL_Event *event)
{
     goat_player *p;
     switch (event->type) {
     case SDL_JOYHATMOTION:
          p = game->p1;
          switch (event->jhat.value) {
          case SDL_HAT_UP:
               p->edit_ptr -= 2;
               break;
          case SDL_HAT_DOWN:
               p->edit_ptr += 2;
               break;
          case SDL_HAT_LEFT:
               p->edit_ptr -= 1;
               break;
          case SDL_HAT_RIGHT:
               p->edit_ptr += 1;
               break;
          }
          break;
     }
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
