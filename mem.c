#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "mem.h"

goat_mem *
make_goat_mem(int mem_size)
{
     goat_mem *gm = 
          (goat_mem *) malloc(sizeof(goat_mem));

     assert(gm != NULL);

     gm->mem_size = mem_size;
     gm->last_read_loc = 0;
     gm->last_write_loc = 0;
     gm->mem = (unsigned char *) calloc(mem_size, sizeof(unsigned char));

     int i;
     for (i = 0; i < mem_size ; i++) {
          gm->mem[i] = i % 255;
     }

     assert(gm->mem != NULL);

     return gm;
}

void free_goat_mem(goat_mem *goat_mem) 
{
     free(goat_mem->mem);
     free(goat_mem);
}

unsigned int goat_mem_normalise(goat_mem *goat_mem, int loc)
{
     if (loc < 0) {
          loc = goat_mem->mem_size + loc;
     }
     
     return (loc % goat_mem->mem_size);
}

unsigned char goat_mem_get(goat_mem *goat_mem, int loc)
{
     unsigned int addr = goat_mem_normalise(goat_mem, loc);

     goat_mem->last_read_loc = addr;

     return *(goat_mem->mem + addr);
}


void goat_mem_put(goat_mem *goat_mem, int loc, unsigned char val)
{
     unsigned int addr = goat_mem_normalise(goat_mem, loc);
     
     goat_mem->last_write_loc = addr;

     *(goat_mem->mem + addr) = val;
}


void goat_mem_inc(goat_mem *goat_mem, int loc, int mod)
{
     unsigned int addr = goat_mem_normalise(goat_mem, loc);
     goat_mem->last_write_loc = addr;
     *(goat_mem->mem + addr) += mod;

}
