#ifndef _GOAT_THREAD_H
#define _GOAT_THREAD_H

#include "mem.h"

typedef enum {
     GOAT_THREAD_AVAILABLE,
     GOAT_THREAD_RUNNING,
     GOAT_THREAD_EXECUTING,
     GOAT_THREAD_DEAD
} goat_thread_status;

typedef struct goat_instruction goat_instruction;
typedef struct goat_thread      goat_thread;

struct goat_instruction {
     unsigned char opcode;
     unsigned int cycles;
     char     *str;
     unsigned char is_branch;
     unsigned char has_arg;

     int (*handler)(goat_mem *mem, goat_thread *thread, unsigned char arg);
};


struct goat_thread {
     unsigned int pc;
     unsigned char a, b, x;

     /* used for executing a multi-cycle instruction.  

        if mem->mem[pc] == multi_cycle_instruction, then the instruction wont
        actually execute until cycle_count == the instruction cycle
        count.

        the currently executing instruction is cached here so the user
        doesn't sneaky switch it mid-instruction.
     */

     goat_instruction *cur_instr;
     unsigned int cycle_count;

     goat_mem *mem;
     goat_thread_status status;
};



goat_thread * make_goat_thread(goat_mem *mem);
goat_thread_status goat_thread_tick(goat_thread *thread);
void free_goat_thread(goat_thread *thread);
goat_instruction * get_goat_instruction(unsigned char opcode);
unsigned int goat_thread_runnable(goat_thread *thread);


/* instructions */
int goat_instr_nop(goat_mem *mem, goat_thread *thread, unsigned char arg);
int goat_instr_mov_a_imm(goat_mem *mem, goat_thread *thread, unsigned char arg);
int goat_instr_mov_b_a(goat_mem *mem, goat_thread *thread, unsigned char arg);
int goat_instr_jmp(goat_mem *mem, goat_thread *thread, unsigned char arg);

#endif

