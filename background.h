#include "init.h"
#include "game.h"

struct Background{

    struct Layers{
        SDL_Texture* look;
        SDL_Rect src;
        int vel;
        int width;
    };

    Layers layer[3];

    Game* game;

    Background(Game* game_){
        game = game_;

        for(int i=0; i<3; i++) layer[i].look = NULL;
    }

    ~Background(){
        game = NULL;

        for(int i=0; i<3; i++) layer[i].look = NULL;
    }
    
    void set_vel(){
        layer[0].vel = 3;
        layer[1].vel = 2;
        layer[2].vel = 1;
    }

    void set_rect(){
        for(int i=0; i<3; i++) layer[i].src = {0, 0, game->RENDER_WIDTH, game->RENDER_HEIGHT};
    }

    void set_width(){
        for(int i=0; i<3; i++) layer[i].width = 1060;
    }

    void loadFromFile(std::string path_0, std::string path_1, std::string path_2){
        SDL_Surface* imgTemp = IMG_Load(path_0.c_str());
        if(imgTemp == NULL) error_i
        layer[0].look = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(layer[0].look == NULL) error

        imgTemp = IMG_Load(path_1.c_str());
        if(imgTemp == NULL) error_i
        layer[1].look = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(layer[1].look == NULL) error

        imgTemp = IMG_Load(path_2.c_str());
        if(imgTemp == NULL) error_i
        layer[2].look = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(layer[2].look == NULL) error

        SDL_FreeSurface(imgTemp);
        imgTemp = NULL;
    }

    void render(){
        for(int i=0; i<3; i++) {
            layer[i].src.x = (layer[i].src.x + layer[i].vel>layer[i].width?0:layer[i].src.x + layer[i].vel);
            SDL_RenderCopy(game->renderer, layer[i].look, &layer[i].src, NULL);
        }
    }
};