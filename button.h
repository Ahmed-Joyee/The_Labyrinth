#pragma once

#include "init.h"
#include "screen.h"
#include "game.h"

struct Button{    

    int buttonID;
    SDL_Texture* look;
    SDL_Rect src;
    SDL_Rect dest;

    Mix_Chunk* click_sound;

    Game* game;

    Button(int buttonID_, Game* game_){
        src = {0, 0, 300, 100};
        dest = {0, 0, 300, 75};

        buttonID = buttonID_;
        game = game_;
    }

    ~Button(){
        SDL_DestroyTexture(look);
    }

    void loadFromFile(std::string path){
        SDL_Surface* imgTemp = IMG_Load(path.c_str());
        if(imgTemp == NULL) error_i
        
        look = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(look == NULL) error

        SDL_FreeSurface(imgTemp);
        imgTemp = NULL;

        click_sound = Mix_LoadWAV("sound/metalClick.wav");
        if(!click_sound) error_m
    }

    void set_dest(int x, int y){
        dest = {x, y, 300, 75};
    }

    bool mouse_in_button(int x, int y){
        return !((x < dest.x) ||
                (x > dest.x + dest.w) ||
                (y < dest.y) ||
                (y > dest.y + dest.h));
    }

    void handle_event(SDL_Event& e){
        int x, y;
        SDL_GetMouseState(&x, &y);
        if(mouse_in_button(x, y)) {
            if(e.type == SDL_MOUSEMOTION) src.x = src.w;
            else if(e.type == SDL_MOUSEBUTTONDOWN) {
                src.x = 2*src.w;
                Mix_PlayChannel(-1, click_sound, 0);
            }
            else if(src.x == 2*src.w && e.button.state == SDL_RELEASED) game->button_action(buttonID), src.x = 0;
        }
        else src.x = 0;
    }

    void render(){
        SDL_RenderCopy(game->renderer, look, &src, &dest);
    }
};