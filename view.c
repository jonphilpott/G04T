
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


     for (i = 0 ; i < 16; i++) {
          unsigned p = s_ptr + (2 * i);
          unsigned int loc = goat_mem_normalise(view->mem, p);
          unsigned char  a = goat_mem_get(view->mem, p);
          unsigned char  b = goat_mem_get(view->mem, p + 1);
          gi = get_goat_instruction(a);
          char *m = (gi == NULL) ? no_instr : gi->str;

          bzero((void *) line_buffer, 32);


          snprintf(line_buffer, 32, "%04X%c%02X%c%02X %s %02Xh", 
                   loc, 
                   (p == ptr) ? '>' : ' ',
                   a, 
                   (p+1 == ptr) ? '>' : ' ',
                   b,
                   m,
                   (gi != NULL && gi->has_arg == 1) ? b : 0
               );
          text_buffer_write(view->text_buffer, view->cx + 1, view->cy + 1 + i, line_buffer, 32);
     }
}

