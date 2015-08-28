#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

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

     srand((unsigned int) time(NULL));
     unsigned int p1_start = (rand() % 512) & (-1 << 1);
     unsigned int p1_end   = goat_mem_normalise(mem, p1_start + 32);

     unsigned int p2_start = goat_mem_normalise(
          mem, 
          p1_end + (rand() % (512-64)) & (-1 << 1)
          );
     unsigned int p2_end = goat_mem_normalise(mem, p2_start + 32);



     goat_player *p1 = make_goat_player(mem, p1_start, p1_end);
     goat_player *p2 = make_goat_player(mem, p2_start, p2_end);

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
     game->tick_interval = GOAT_GAME_TICK_INTERVAL_MS;
     game->next_tick = SDL_GetTicks() + game->tick_interval;
     game->player_tick_interval = GOAT_GAME_PLAYER_TICK_INTERVAL;
     game->player_next_tick = SDL_GetTicks() + game->player_tick_interval;
     
         
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
     Uint32 t = SDL_GetTicks();

     if (t >= game->next_tick) {
          goat_player *player = (game->ctx & 1) ? game->p1 : game->p2;
          goat_player_tick(player);
          game->ctx++;
          game->next_tick = t + game->tick_interval;
     }

     if (t >= game->player_next_tick) {
          game->player_next_tick = t + game->player_tick_interval;
          
          game->p1->edit_ptr += game->p1->edit_d;
          game->p2->edit_ptr += game->p2->edit_d;

          if ( !goat_player_loc_protected(game->p2, game->p1->edit_ptr)) {
               goat_mem_inc(game->mem, game->p1->edit_ptr, game->p1->mem_d);
          }

          if ( !goat_player_loc_protected(game->p1, game->p1->edit_ptr)) {
               goat_mem_inc(game->mem, game->p2->edit_ptr, game->p2->mem_d);
          }
     }
}


void goat_game_handle_input(goat_game *game, SDL_Event *event)
{
     goat_player *p;
     switch (event->type) {
     case SDL_JOYHATMOTION:
          p = event->jhat.which ? game->p2 : game->p1;
          switch (event->jhat.value) {
          case SDL_HAT_UP:
               p->edit_d = -2;
               break;
          case SDL_HAT_DOWN:
               p->edit_d = 2;
               break;
          case SDL_HAT_LEFT:
               p->edit_d = -1;
               break;
          case SDL_HAT_RIGHT:
               p->edit_d = +1;
               break;
          case SDL_HAT_CENTERED:
               p->edit_d = 0;
               break;
          }
          break;
     case SDL_JOYAXISMOTION:
          p = event->jaxis.which ? game->p2 : game->p1;
          printf("axis = %u value = %u\n", event->jaxis.axis, event->jaxis.value);
          if (event->jaxis.value > 0) {
               p->edit_d = (event->jaxis.axis) ? 2 : 1;
          }
          else if (event->jaxis.value < 0) {
               p->edit_d = (event->jaxis.axis) ? -2 : -1;
          }
          else {
               p->edit_d = 0;
          }
          break;
     case SDL_JOYBUTTONDOWN:
          p = event->jbutton.which ? game->p2 : game->p1;
          switch (event->jbutton.button) {
          case 0:
               p->mem_d = 1;
               break;
          case 1:
               p->mem_d = -1;
          }
          break;
     case SDL_JOYBUTTONUP:
          p = event->jbutton.which ? game->p2 : game->p1;
          switch (event->jbutton.button) {
          case 0:
          case 1:
               p->mem_d = 0;
               break;
          case 2:
               goat_player_spawn_thread(p, p->edit_ptr);
               break;
          }
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
