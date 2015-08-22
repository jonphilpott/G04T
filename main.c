#include <SDL.h>
#include "sdl.h"

#include "game.h"

int main(int argc, char *argv[])
{
     SDL_Surface *screen = sdl_init();
     
     goat_game *game = make_goat_game();

     sdl_main_loop(screen, game);

     return 0;
}

