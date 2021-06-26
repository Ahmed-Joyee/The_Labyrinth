#include "init.h"
#include "game.h"
#include "tile.h"
#include "player.h"
#include "enemy.h"
#include "screen.h"

int main(int argc, char* argv[]) {

    //initialize game. All the variables everything else requires initializes with it
    Game* game = new Game();
    game->init("The Labrynth");
    game->text_loader(46);

    //gameloop deciding what screen to render
    while (game->game_running) {
        if(game->current_screen == game->UI_SCREEN) UI(game);
        else if(game->current_screen >= game->INSTRUCTIONS_SCREEN_0 && game->current_screen <= game->INSTRUCTIONS_SCREEN_2) instructions(game);
        else if(game->current_screen == game->OPTIONS_SCREEN) options(game);
        else if(game->current_screen == game->LEVEL_CHOICE) levels(game);
        else if(game->current_screen == game->HISCORE_SCREEN) hiscore(game);
        else if(game->current_screen == game->CREDITS_SCREEN) credits(game);
        else if(game->current_screen == game->LEVEL_1) level_one(game);
        else if(game->current_screen == game->LEVEL_2) level_two(game);
        else if(game->current_screen == game->LEVEL_3) level_three(game);
    }

    game->close();
    return 0;
}