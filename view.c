
#include <SDL.h>
#include <stdlib.h>
#include <assert.h>

#include "player.h"
#include "mem.h"
#include "thread.h"
#include "view.h"

text_buffer *
make_text_buffer(int w, int h) 
{
     text_buffer *tb = 
          (text_buffer *) malloc(sizeof(text_buffer));

     assert(tb != NULL);

     unsigned char *b = 
          (unsigned char *) malloc(w * h);

     bzero(b, w*h);

     assert(b != NULL);

     tb->w = w; 
     tb->h = h;
     tb->buffer = b;
     
     return tb;
}

void free_text_buffer(text_buffer *text_buffer)
{
     free(text_buffer->buffer);
     free(text_buffer);
}

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

void text_buffer_draw_box(text_buffer *text_buffer, int cx, int cy, int w, int h)
{
     int x;
     int y;

     // top & bottom line
     for (x = cx+1 ; x < cx+w; x++) {
          text_buffer->buffer[(cy * text_buffer->w) + x] = '\xcd';
          text_buffer->buffer[((cy + h) * text_buffer->w) + x] = '\xcd';
     }

     // sides
     for (y = cy+1 ; y < cy+h; y++) {
          text_buffer->buffer[(y * text_buffer->w) + cx] = '\xba';
          text_buffer->buffer[(y * text_buffer->w) + cx + w] = '\xba';
     } 

     // corners
     text_buffer->buffer[(cy * text_buffer->w) + cx]           = '\xc9';
     text_buffer->buffer[(cy * text_buffer->w) + cx + w]       = '\xbb';
     text_buffer->buffer[((cy + h) * text_buffer->w) + cx]     = '\xc8';
     text_buffer->buffer[((cy + h) * text_buffer->w) + cx + w] = '\xbc';

}


void text_buffer_write(text_buffer *text_buffer, int x, int y, char *str, size_t n)
{
     unsigned int offset = (y * text_buffer->w) + x;
     memcpy(text_buffer->buffer + offset, str, n);
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
     text_buffer_draw_box(view->text_buffer, view->cx, view->cy + 23, 33, 10);

     for (i = 0 ; i < 21; i++) {
          unsigned p = s_ptr + (2 * i);
          unsigned int loc = goat_mem_normalise(view->mem, p);
          unsigned char  a = goat_mem_get(view->mem, p);
          unsigned char  b = goat_mem_get(view->mem, p + 1);
          gi = get_goat_instruction(a);
          char *m = (gi == NULL) ? no_instr : gi->str;

          bzero((void *) line_buffer, 32);


          snprintf(line_buffer, 32, "%04X%c%02X%c%02X %s %02Xh", 
                   loc, 
                   (p == ptr) ? '\x10' : ' ',
                   a, 
                   (p+1 == ptr) ? '\x10' : ' ',
                   b,
                   m,
                   (gi != NULL && gi->has_arg == 1) ? b : 0
               );
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

