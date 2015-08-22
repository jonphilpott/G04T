#include <stdlib.h>
#include <assert.h>

#include "thread.h"
#include "mem.h"



goat_instruction instructions[] = {
     //op  cycles  str         branch?  has_arg?  handler
     {1,   1,      "NOP",      0,       0,        &goat_instr_nop        },
     {2,   1,      "MOV A, ",  0,       1,        &goat_instr_mov_a_imm  },
     {3,   1,      "MOV B, A", 0,       0,        &goat_instr_mov_b_a    },
     {4,   2,      "JMP ",     1,       1,        &goat_instr_jmp        }
};

/* will break if this is wrong */
unsigned int n_instructions = 4;

goat_instruction *
get_goat_instruction(unsigned char opcode) 
{
     if (opcode > 0 && opcode <= n_instructions) {
          return &instructions[opcode-1];
     }
     else {
          return NULL;
     }
}

int goat_instr_nop(goat_mem *mem, goat_thread *thread, unsigned char arg) 
{
     return 2;
}

int goat_instr_mov_a_imm(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = arg;
     return 2;
}

int goat_instr_mov_b_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->b = thread->a;
     return 2;
}

int goat_instr_jmp(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     signed char a = (signed char) arg;
     return a;
}

goat_thread *
make_goat_thread(goat_mem *mem)
{
     goat_thread *gt = (goat_thread *) malloc(sizeof(goat_thread));

     assert(gt != NULL);

     gt->pc = gt->cycle_count = 0;
     gt->a = gt->b = gt->x = 0;
     gt->mem = mem;
     gt->status = GOAT_THREAD_AVAILABLE;

     return gt; 
}


goat_thread_status 
goat_thread_tick(goat_thread *thread) 
{
     goat_mem *gm = thread->mem;
     
     if (thread->status != GOAT_THREAD_RUNNING &&
          thread->status != GOAT_THREAD_EXECUTING) {
          return thread->status;
     }

     thread->cycle_count++;
     

     unsigned char opcode = goat_mem_get(gm, thread->pc);
     unsigned char arg    = goat_mem_get(gm, thread->pc + 1);

     if (thread->status == GOAT_THREAD_RUNNING) {
          goat_instruction *gi = get_goat_instruction(opcode);

          if (gi == NULL) {
               thread->status == GOAT_THREAD_DEAD;
               return thread->status;
          }

          thread->cur_instr = gi;
     }

     if (thread->cycle_count < thread->cur_instr->cycles) {
          thread->status = GOAT_THREAD_EXECUTING;
          return thread->status;
     }
     else {
          thread->status = GOAT_THREAD_RUNNING;
          thread->cycle_count = 0;
          int pc_incr = thread->cur_instr->handler(gm, thread, arg);
          thread->pc = goat_mem_normalise(gm, thread->pc + pc_incr);
     }

     return thread->status;
}

unsigned int goat_thread_runnable(goat_thread *thread)
{
     return (thread->status == GOAT_THREAD_RUNNING ||
             thread->status == GOAT_THREAD_EXECUTING);
}


/*
 * does NOT free the goat_mem
 */
void free_goat_thread(goat_thread *thread)
{
     free(thread);
}

