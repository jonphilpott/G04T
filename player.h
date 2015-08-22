#ifndef _GOAT_PLAYER_H
#define _GOAT_PLAYER_H

#include "mem.h"
#include "thread.h"


typedef struct goat_player goat_player;

#define GOAT_MAX_THREADS 4

struct goat_player {
     goat_thread *threads[GOAT_MAX_THREADS];
     unsigned int cur_thread;
     unsigned int available_threads;
     unsigned int running_threads;


     unsigned int pz_start;
     unsigned int pz_end;

     /* the current location of the users edit cursor */
     unsigned int edit_ptr;

};


goat_player * make_goat_player(goat_mem *mem, unsigned int pz_start, unsigned int pz_end);
void free_goat_player(goat_player *player);
goat_thread_status goat_player_tick(goat_player *player);
unsigned int goat_player_spawn_thread(goat_player *p, unsigned int location);

#endif
