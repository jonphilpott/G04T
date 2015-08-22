#ifndef _GOAT_MEM_H
#define _GOAT_MEM_H


typedef struct {
     unsigned int mem_size;
     unsigned int last_read_loc;
     unsigned int last_write_loc;
     unsigned char *mem;
} goat_mem;

goat_mem * make_goat_mem(int mem_size);
void free_goat_mem(goat_mem *goat_mem);
unsigned int goat_mem_normalise(goat_mem *goat_mem, int loc);
unsigned char goat_mem_get(goat_mem *goat_mem, int loc);
void goat_mem_put(goat_mem *goat_mem, int loc, unsigned char val);
void goat_mem_inc(goat_mem *goat_mem, int loc, int mod);

#endif
