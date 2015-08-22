#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "mem.h"
#include "thread.h"
#include "player.h"

int main(int argc, char *argv[]) 
{

     printf("Testing mem functions.\n");
     goat_mem *gm = make_goat_mem(512);

     assert(gm != NULL);
     assert(gm->mem_size == 512);
     assert(gm->last_read_loc == 0);
     assert(gm->last_write_loc == 0);

     assert(goat_mem_normalise(gm, 512) == 0);
     assert(goat_mem_normalise(gm, 513) == 1);
     assert(goat_mem_normalise(gm, -1) == 511);
     assert(goat_mem_normalise(gm, 42) == 42);
     assert(goat_mem_normalise(gm, 0) == 0);


     goat_mem_put(gm, 512, 1);
     goat_mem_put(gm, 513, 2);
     goat_mem_put(gm, -1, 3);
     goat_mem_put(gm, 42, 4);

     assert(goat_mem_get(gm, 512) == 1);
     assert(goat_mem_get(gm, 513) == 2);
     assert(goat_mem_get(gm, -1)  == 3);
     assert(goat_mem_get(gm, 42)  == 4);

     printf("Testing thread functions.\n");

     goat_thread *thread = make_goat_thread(gm);

     assert(thread->mem == gm);
     assert(thread->status == GOAT_THREAD_AVAILABLE);

     goat_instruction *instruction = get_goat_instruction(1);
     assert(instruction != NULL);
     assert(instruction->cycles == 1);
     assert(strcmp(instruction->str, "NOP") == 0);

     instruction = get_goat_instruction(250);
     assert(instruction == NULL);

     printf("Testing thread tick.\n");


     /* execute NOP*/
     goat_mem_put(gm, 0, 1);
     goat_mem_put(gm, 1, 0);
     thread->pc = 0;
     thread->status = GOAT_THREAD_RUNNING;
     goat_thread_tick(thread);
     assert(thread->pc == 2);
     assert(thread->status = GOAT_THREAD_RUNNING);


     /* execute MOV A, 42 */
     goat_mem_put(gm, 0, 2);
     goat_mem_put(gm, 1, 42);
     thread->pc = 0;
     goat_thread_tick(thread);
     assert(thread->pc == 2);
     assert(thread->status == GOAT_THREAD_RUNNING);
     assert(thread->a == 42);

     /* execute MOV B, A */
     goat_mem_put(gm, 0, 3);
     thread->pc = 0;
     goat_thread_tick(thread);
     assert(thread->pc == 2);
     assert(thread->status == GOAT_THREAD_RUNNING);
     assert(thread->b == 42);
     assert(thread->a == 42);

     /* execute JMP 4 */
     goat_mem_put(gm, 0, 4);
     goat_mem_put(gm, 1, 4);
     thread->pc = 0;
     goat_thread_tick(thread);
     /* status should be EXECUTING and pc == 0, JMP is 2 cycles */
     assert(thread->status == GOAT_THREAD_EXECUTING);
     assert(thread->pc == 0);
     /* tick again */
     goat_thread_tick(thread);
     assert(thread->pc == 4);
     assert(thread->status == GOAT_THREAD_RUNNING);

     /* execute JMP -4 */
     goat_mem_put(gm, 0, 4);
     goat_mem_put(gm, 1, -4);
     thread->pc = 0;
     goat_thread_tick(thread);
     /* status should be EXECUTING and pc == 4, JMP is 2 cycles */
     assert(thread->status == GOAT_THREAD_EXECUTING);
     assert(thread->pc == 0);
     /* tick again */
     goat_thread_tick(thread);
     assert(thread->pc == 508);
     assert(thread->status == GOAT_THREAD_RUNNING);

     printf("Testing player functions.\n");
     
     goat_player *player = make_goat_player(gm, 0, 64);
     assert(player != NULL);
     assert(player->pz_start == 0);
     assert(player->pz_end   == 64);
     assert(player->running_threads == 0);
     assert(player->available_threads = GOAT_MAX_THREADS);

     int i;

     for (i = 0; i < GOAT_MAX_THREADS; i++) {
          assert(player->threads[i] != NULL);
     }

     /* put some code back at 0 */
     goat_mem_put(gm, 0, 2);
     goat_mem_put(gm, 1, 42);

     int r = goat_player_spawn_thread(player, 0);
     assert(r == 1);
     assert(player->available_threads == (GOAT_MAX_THREADS-1));
     assert(player->running_threads == 1);
     assert(player->threads[0]->status == GOAT_THREAD_RUNNING);

     assert(goat_player_tick(player) == GOAT_THREAD_RUNNING);
     assert(player->threads[0]->a == 42);
     assert(player->threads[0]->pc == 2);

     /* spawn the rest of the threads */
     for (i = 1; i < GOAT_MAX_THREADS; i++) {
          r = goat_player_spawn_thread(player, 0);
          assert(r == 1);
     }
     
     assert(player->available_threads == 0);
     assert(player->running_threads == GOAT_MAX_THREADS);

     /* tick the rest of the threads */
     for (i = 1; i < GOAT_MAX_THREADS; i++) {
          assert(goat_player_tick(player) == GOAT_THREAD_RUNNING);
     }

     /* make sure all threads are still running and have the right values */
     for (i = 0; i < GOAT_MAX_THREADS; i++) {
          assert(player->threads[i]->status == GOAT_THREAD_RUNNING);
          assert(player->threads[i]->pc == 2);
          assert(player->threads[i]->a == 42);
     }

     exit(0);
}


