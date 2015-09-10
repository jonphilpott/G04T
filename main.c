#include <SDL.h>
#include <assert.h>

#include "sdl.h"

#include "game.h"


int sdl_main_loop(goat_sdl *gsdl, goat_game *game);


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


int sdl_main_loop(goat_sdl *gsdl, goat_game *game)
{
     SDL_Surface *screen = gsdl->screen;
     int keep_going = 1;
     SDL_Event event;

     while (keep_going) {
          if (SDL_PollEvent(&event)) {
               goat_game_handle_input(game, &event);
               switch(event.type) {
               case SDL_QUIT:
                    keep_going = 0;
                    break;
               case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                         keep_going = 0;
                         break;
                    }
               }
          }
          
          goat_game_tick(game);
          goat_player_view_refresh(game->p1_view);
          goat_player_view_refresh(game->p2_view);
          goat_game_view_refresh(game);
          sdl_draw_text_buffer(game->text_buffer, screen);
          SDL_UpdateRect(screen, 0, 0, 0, 0);
          SDL_Flip(screen);
     }

     SDL_Quit();
     return 0;
}
