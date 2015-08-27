#include <stdlib.h>
#include <assert.h>

#include "thread.h"
#include "mem.h"



goat_instruction instructions[] = {
     //op  cycles  str              signed?  has_arg?  handler
     {1,   1,      "NOP ",          0,       0,        &goat_instr_nop             },
     {2,   1,      "MOV  A,",       0,       1,        &goat_instr_mov_a_imm       },
     {3,   1,      "MOV  B, A",     0,       0,        &goat_instr_mov_b_a         },
     {4,   1,      "XCHG B, X",     0,       0,        &goat_instr_xchg_b_x        },
     {5,   1,      "XCHG A, B",     0,       0,        &goat_instr_xchg_a_b        },
     {6,   1,      "ADD  A, B",     0,       0,        &goat_instr_add_a_b         },
     {7,   1,      "SUB  A, B",     0,       0,        &goat_instr_sub_a_b         }, 
     {8,   2,      "MOV  A, [B]",   0,       0,        &goat_instr_mov_a_indr_b    },
     {9,   2,      "MOV  [B], A",   0,       0,        &goat_instr_mov_indr_b_a    },
     {10,  3,      "MOV  A:B, [X]", 0,       0,        &goat_instr_mov16_ab_indr_x },
     {11,  3,      "MOV  [X], A:B", 0,       0,        &goat_instr_mov16_indr_x_ab },
     {12,  1,      "INC  A",        0,       0,        &goat_instr_inc_a           },
     {13,  1,      "INC  B",        0,       0,        &goat_instr_inc_b           },
     {14,  1,      "INC  *",        0,       1,        &goat_instr_inc_indr        },
     {15,  1,      "INC  [B]",      0,       0,        &goat_instr_inc_indr_b      },
     {16,  1,      "DEC  A",        0,       0,        &goat_instr_dec_a           },
     {17,  1,      "DEC  B",        0,       0,        &goat_instr_dec_b           },
     {18,  1,      "DEC  *",        0,       1,        &goat_instr_dec_indr        },
     {19,  1,      "DEC  [B]",      0,       0,        &goat_instr_dec_indr_b      },
     {20,  1,      "SHL  A,",       0,       1,        &goat_instr_shl_a           },
     {21,  1,      "SHR  A,",       0,       1,        &goat_instr_shr_a           },
     {22,  2,      "JMP ",          1,       1,        &goat_instr_jmp             },
     {23,  2,      "JZ  ",          1,       1,        &goat_instr_jz              },
     {24,  2,      "JNZ ",          1,       1,        &goat_instr_jnz             },
     {25,  3,      "DJNZ",          1,       1,        &goat_instr_djnz            },
     {26,  1,      "AND  A, B",     0,       0,        &goat_instr_and_a_b         },
     {27,  1,      "OR   A, B",     0,       0,        &goat_instr_or_a_b          },
     {28,  1,      "XOR  A, B",     0,       0,        &goat_instr_xor_a_b         },
     {29,  1,      "NEG  A",        0,       0,        &goat_instr_neg_a           },
     {30,  1,      "NOT  A",        0,       0,        &goat_instr_not_a           },
     {31,  1,      "IN   A",        0,       0,        &goat_instr_in_a            },
     {32,  1,      "OUT  A,",       0,       1,        &goat_instr_out_a           }
};

/* will break if this is wrong */
unsigned int n_instructions = 32;

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

int goat_instr_xchg_b_x(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     int t = thread->b;
     thread->b = thread->x;
     thread->x = t;
     return 2;
}

int goat_instr_xchg_a_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     int t = thread->a;
     thread->b = thread->b;
     thread->b = t;
     return 2;
}

int goat_instr_add_a_b(goat_mem *mem, goat_thread *thread, unsigned char arg) 
{
     thread->a += thread->b;
     return 2;
}

int goat_instr_sub_a_b(goat_mem *mem, goat_thread *thread, unsigned char arg) 
{
     thread->a -= thread->b;
     return 2;
}

int goat_instr_mov_a_indr_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = goat_mem_get(mem, thread->pc + thread->b);
     return 2;
}

int goat_instr_mov_indr_b_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     goat_mem_put(mem, thread->pc + thread->b, thread->a);
     return 2;
}

int goat_instr_mov16_ab_indr_x(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = goat_mem_get(mem, thread->x + thread->pc);
     thread->b = goat_mem_get(mem, thread->x + thread->pc + 1);
     return 2;
}

int goat_instr_mov16_indr_x_ab(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     goat_mem_put(mem, thread->x + thread->pc, thread->a);
     goat_mem_put(mem, thread->x + thread->pc + 1, thread->b);
     
     return 2;
}

int goat_instr_inc_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a++;
     return 2;
}

int goat_instr_inc_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->b++;
     return 2;
}

int goat_instr_inc_indr(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     goat_mem_inc(mem, thread->pc + arg, 1);
     return 2;
}

int goat_instr_inc_indr_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     goat_mem_inc(mem, thread->pc + thread->b, 1);
     return 2;
}

int goat_instr_dec_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a--;
     return 2;
}

int goat_instr_dec_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->b--;
     return 2;
}

int goat_instr_dec_indr(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     goat_mem_inc(mem, thread->pc + arg, -1);
     return 2;
}

int goat_instr_dec_indr_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     goat_mem_inc(mem, thread->pc + thread->b, -1);
     return 2;
}

int goat_instr_shl_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = (thread->a << arg);
     return 2;
}

int goat_instr_shr_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = (thread->a >> arg);
     return 2;
}

int goat_instr_jmp(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     signed char a = (signed char) arg;
     return a;
}

int goat_instr_jz(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     if (thread->a == 0) {
          return (signed char) arg;
     }
     return 2;
}

int goat_instr_jnz(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     if (thread->a != 0) {
          return (signed char) arg;
     }

     return 2;
}

int goat_instr_djnz(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a--;
     
     if (thread->a != 0) {
          return (signed char) arg;
     }
     
     return 2;
}

int goat_instr_and_a_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a &= thread->b;
     return 2;
}

int goat_instr_or_a_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a |= thread->b;
     return 2;
}

int goat_instr_xor_a_b(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a ^= thread->b;
     return 2;
}

int goat_instr_neg_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = (signed char) thread->a * -1;
     return 2;
}

int goat_instr_not_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = ~(thread->a);
     return 2;
}

int goat_instr_in_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     thread->a = 42;
     return 2;
}

int goat_instr_out_a(goat_mem *mem, goat_thread *thread, unsigned char arg)
{
     return 2;
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
               thread->status = GOAT_THREAD_DEAD;
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

