
#include <SDL.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "game.h"
#include "player.h"
#include "mem.h"
#include "thread.h"
#include "view.h"
#include "sdl.h"


void free_goat_player_view(goat_player_view *view)
{
     free(view);
}

goat_player_view *
make_goat_player_view(goat_player *player,
                      goat_player *opponent,
                      goat_mem    *mem,
                      unsigned int cx,
                      unsigned int cy,
                      text_buffer *text_buffer)
{
     goat_player_view *gpv =
          (goat_player_view *) malloc(sizeof(goat_player_view));

     assert(gpv != NULL);

     gpv->player = player;
     gpv->opponent = opponent;
     gpv->mem = mem;
     gpv->cx = cx;
     gpv->cy = cy;
     gpv->text_buffer = text_buffer;

     return gpv;
}


unsigned int goat_player_has_thread_here(goat_player *player, unsigned int loc)
{
     int i;
     for (i = 0 ; i < GOAT_MAX_THREADS ; i++) {
          goat_thread *t = player->threads[i];
          int tl = t->pc & (-1 << 1);
          if (goat_thread_runnable(t) && tl == loc) {
               return 1;
          }
     }

     return 0;
}

void goat_player_view_refresh(goat_player_view *view)
{

     unsigned int ptr = view->player->edit_ptr;
     int i;

     /* round it off to the nearest memory pair */
     unsigned int s_ptr = (ptr & (-1 << 1)) - 16;

     unsigned char line_buffer[32];

     char *no_instr = "???";
     goat_instruction *gi;

     text_buffer_draw_box(view->text_buffer, view->cx, view->cy, 33, 22);
     text_buffer_draw_box(view->text_buffer, view->cx, view->cy + 23, 33, 6);

     for (i = 0 ; i < 21; i++) {
          unsigned p = s_ptr + (2 * i);
          unsigned int loc = goat_mem_normalise(view->mem, p);
          unsigned char  a = goat_mem_get(view->mem, p);
          unsigned char  b = goat_mem_get(view->mem, p + 1);
          gi = get_goat_instruction(a);
          char *m = (gi == NULL) ? no_instr : gi->str;

          bzero((void *) line_buffer, 32);
          
          /* TODO: Make this suck less. */
		
          if (gi != NULL && gi->has_arg) {
               if (gi->is_arg_signed) {
                    snprintf(line_buffer, 32, "%c%c%04X%c%02X%c%02X %s %hhd",
                             goat_player_has_thread_here(view->opponent, loc) ? 'E' : ' ',
                             goat_player_has_thread_here(view->player,   loc) ? '[' : ' ',
                             loc, 
                             (p == ptr) ? '\x10' : ' ',
                             a, 
                             (p+1 == ptr) ? '\x10' : ' ',
                             b,
                             m,
                             (signed char) b
                         );
               }
               else {
                    snprintf(line_buffer, 32, "%c%c%04X%c%02X%c%02X %s %02Xh",
                             goat_player_has_thread_here(view->opponent, loc) ? 'E' : ' ',
                             goat_player_has_thread_here(view->player,   loc) ? '[' : ' ',
                             loc, 
                             (p == ptr) ? '\x10' : ' ',
                             a, 
                             (p+1 == ptr) ? '\x10' : ' ',
                             b,
                             m,
                             b
                         );
               }
          } 
          else {
               snprintf(line_buffer, 32, "%c%c%04X%c%02X%c%02X %s",
                        goat_player_has_thread_here(view->opponent, loc) ? 'E' : ' ',
                        goat_player_has_thread_here(view->player,   loc) ? '[' : ' ',
                        loc, 
                        (p == ptr) ? '\x10' : ' ',
                        a, 
                        (p+1 == ptr) ? '\x10' : ' ',
                        b,
                        m
                    );
          }
          text_buffer_write(view->text_buffer, view->cx + 1, view->cy + 1 + i, line_buffer, 32);
     }

     text_buffer_write(view->text_buffer, view->cx + 1, view->cy + 24,
                       "CPU REGS: ", 10);

     for (i = 0; i < GOAT_MAX_THREADS; i++) {
          goat_thread *thread = view->player->threads[i];
          bzero((void *) line_buffer, 32);
          char st = '?';

          switch (thread->status) {
          case GOAT_THREAD_AVAILABLE: st = 'A'; break;
          case GOAT_THREAD_RUNNING:   st = 'R'; break;
          case GOAT_THREAD_EXECUTING: st = 'E'; break;
          case GOAT_THREAD_DEAD:      st = 'X'; break;
          }


          snprintf(line_buffer, 32, "%01X\xaf%c P:%04Xh A:%02Xh B:%02Xh X:%02Xh",
                   i,
                   st,
                   thread->pc,
                   thread->a,
                   thread->b,
                   thread->x
               );
          text_buffer_write(view->text_buffer, 
                            view->cx + 1, 
                            view->cy + 24 + 1 + i, 
                            line_buffer,
                            32);

     }

}

