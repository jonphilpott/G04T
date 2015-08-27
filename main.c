#include <SDL.h>
#include <assert.h>

#include "sdl.h"

#include "game.h"

int main(int argc, char *argv[])
{
     goat_sdl *sdl = sdl_init();

     if (sdl == NULL) {
          return 1;
     }

     goat_game *game = make_goat_game();

     sdl_main_loop(sdl, game);

     free_goat_game(game);

     return 0;
}


