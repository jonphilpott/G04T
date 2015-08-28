#include <stdlib.h>
#include <assert.h>


#include "mem.h"
#include "thread.h"
#include "player.h"

void free_goat_player(goat_player *player)
{
     /* free up the threads */
     int i;
     for (i = 0; i < GOAT_MAX_THREADS; i++) {
          free_goat_thread(player->threads[i]);
     }
     free(player);
}

goat_player *
make_goat_player(goat_mem *mem, unsigned int pz_start, unsigned int pz_end)
{
     goat_player *p = 
          (goat_player *) malloc(sizeof(goat_player));

     assert(p != NULL);

     p->cur_thread = 0;
     p->available_threads = GOAT_MAX_THREADS;
     p->running_threads = 0;
     p->pz_start = pz_start;
     p->pz_end = pz_end;
     p->edit_ptr = pz_start;
     p->edit_d = 0;
     p->mem_d = 0;
     
     unsigned int i;

     for (i = 0 ; i < GOAT_MAX_THREADS; i++) {
          p->threads[i] = make_goat_thread(mem);
          p->threads[i]->pc = pz_start;
     }

     return p;
}


unsigned int goat_player_spawn_thread(goat_player *p, unsigned int location)
{
     if (p->available_threads < 1) {
          return 0;
     }

     goat_thread *t = NULL;
     unsigned int i;

     for (i = 0 ; i < GOAT_MAX_THREADS ; i++) {
          if (p->threads[i]->status == GOAT_THREAD_AVAILABLE) {
               t = p->threads[i];
               break;
          }
     }

     assert(t != NULL);
     p->available_threads--;
     p->running_threads++;
     t->pc = location;
     t->status = GOAT_THREAD_RUNNING;
     return 1;
}

goat_thread_status goat_player_tick(goat_player *player) 
{
     unsigned int i;
     goat_thread *t = NULL;
     
     if (player->running_threads < 1) {
          return GOAT_THREAD_DEAD;

     }

     /* find a thread */
     for (i = 0 ; i < GOAT_MAX_THREADS; i++) {
          unsigned int c = (player->cur_thread + i) % GOAT_MAX_THREADS;
          if (goat_thread_runnable(player->threads[c])) {
               t = player->threads[c];
               player->cur_thread = c;
               break;
          }
     }

     assert(t != NULL);
     goat_thread_tick(t);

     if (t->status == GOAT_THREAD_DEAD) {
          player->running_threads--;
     }
     
     player->cur_thread++;
     return t->status;
}

unsigned int goat_player_loc_protected(goat_player *player, unsigned int loc)
{
     if (loc >= player->pz_start && loc <= player->pz_end) {
          return 1;
     }

     return 0;
}
