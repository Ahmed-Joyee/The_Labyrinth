#pragma once

#include "init.h"
#include "game.h"


struct Tile {

    enum tile_identity {
        TILE_WALL0,
        TILE_TREE0,
        TILE_WALL2,
        TILE_TREE1,
        TILE_WALL4,

        TILE_WALK0,
        TILE_WALK1,
        TILE_WALK2,
        TILE_WALK3,
        TILE_WALK4,

        TILE_BUTTON_DEFAULT,
        TILE_BUTTON_TRIGGERED,

        TILE_GATE_DEFAULT,
        TILE_GATE_OPEN,

        TILE_CHEST_DEFAULT,
        TILE_CHEST_OPEN,

        TILE_TOTAL
    };

    SDL_Texture* tile_image;
    SDL_Texture* chest_image;
    SDL_Texture* button_image;
    SDL_Texture* gate_image;

    SDL_Rect tiles[TILE_TOTAL];

    Mix_Chunk* door_open_sound;

    int tile_width, tile_height;
    int tile_type[150][200];
    float angle[150][120];
    int slow_factor;
    int music_channel;

    Game* game;

    //array par[i] associates i'th switch to i'th gate. for switch i, par[i] has it's gate's column and row
    struct Gate_switch_parent {
        int gate_row[5];
        int gate_col[5];
        int button_row;
        int button_col;
        int cnt;  //number of gates associated with switch

        Gate_switch_parent() {
            cnt = 0;
        }
    } par[10];

    Tile(int w, int h, Game* game_) {
        tile_width = w;
        tile_height = h;
        slow_factor = 6;

        game = game_;

        music_channel = game->sound_channel[game->SFX_CHANNEL_3];

        for(int i=0; i<5; i++) tiles[i] = {32*i, 0, 32, 32};
        tiles[TILE_TREE0].h = 42;
        tiles[TILE_TREE1].h = 44;
        
        for(int i=5; i<10; i++) tiles[i] = {32*(i-5), 45, 32, 32};

        tiles[TILE_BUTTON_DEFAULT] = {0, 0, 32, 32};
        tiles[TILE_BUTTON_TRIGGERED] = {32, 0, 32, 32};

        tiles[TILE_GATE_DEFAULT] = {0, 0, 5, 32};
        tiles[TILE_GATE_OPEN] = {0, 0, 5, 32};

        tiles[TILE_CHEST_DEFAULT] = {0, 0, 42, 32};
        tiles[TILE_CHEST_OPEN] = {42, 0, 42, 32};
        
    }

    ~Tile(){
        SDL_DestroyTexture(tile_image);
        tile_image = NULL;
        SDL_DestroyTexture(chest_image);
        chest_image = NULL;
        SDL_DestroyTexture(button_image);
        button_image = NULL;
        SDL_DestroyTexture(gate_image);
        gate_image = NULL;

        Mix_FreeChunk(door_open_sound);
        door_open_sound = NULL;
        
        game = NULL;
    }

    //loads tilesheep from file
    void loadImageFromFile() {

        char tile_path[] = "png/tile/tileset.png";
        char chest_path[] = "png/tile/chest.png";
        char button_path[] = "png/tile/button.png";
        char gate_path[] = "png/tile/gate.png";
        char door_open_sound_path[] = "sound/doorOpen_1.wav";
        
        tile_image = game->texture_loader(tile_path);
        
        chest_image = game->texture_loader(chest_path);
        
        button_image = game->texture_loader(button_path);
        
        gate_image = game->texture_loader(gate_path);
        
        door_open_sound = Mix_LoadWAV(door_open_sound_path);
        if(!door_open_sound) error_m
    }

    //loads map info. What tile goes where. Which tile from the tilesheet goes where
    void loadInfoFromFile(std::string path, int w, int h) {
        std::ifstream map(path);

        if (map.fail()) printf("Failed to load map\n");

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                map >> tile_type[i][j];

                if(tile_type[i][j] == TILE_WALL0){
                    if(tile_type[i+1][j] == TILE_WALL0 || tile_type[i-1][j] == TILE_WALL0)
                        angle[i][j] = 90.0;
                }

                else if (tile_type[i][j] % 100 == TILE_GATE_DEFAULT) {
                    int parent_gate = tile_type[i][j] / 100;
                    par[parent_gate].gate_row[par[parent_gate].cnt] = i;
                    par[parent_gate].gate_col[par[parent_gate].cnt++] = j;

                    if(tile_type[i-1][j] % 100 <= TILE_WALL4 || tile_type[i-1][j] % 100 == TILE_GATE_DEFAULT)
                        angle[i][j] = 0.0;
                    else 
                        angle[i][j] = 90.0;
                } 

                else if (tile_type[i][j] % 100 == TILE_BUTTON_DEFAULT) {
                    int parent_button = tile_type[i][j] / 100;
                    par[parent_button].button_row = i;
                    par[parent_button].button_col = j;
                }
                else angle[i][j] = 0.0;
            }
        }
        map.close();
    }

    //detects and entity's collision with gate or wall. trims the entity's dimensions so it can fit into smaller spaces
    bool tile_gate_wall_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;

        return ((topleft <= TILE_WALL4 || topleft == TILE_GATE_DEFAULT) ||
                (topright <= TILE_WALL4 || topright == TILE_GATE_DEFAULT) ||
                (botleft <= TILE_WALL4 || botleft == TILE_GATE_DEFAULT) ||
                (botright <= TILE_WALL4 || botright == TILE_GATE_DEFAULT));
    }

    //detects and entity's collision with wall. trims the entity's dimensions so it can fit into smaller spaces
    bool tile_wall_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;

        return (topleft <= TILE_WALL4 ||
                topright <= TILE_WALL4 ||
                botleft <= TILE_WALL4 ||
                botright <= TILE_WALL4);
    }

    bool tile_chest_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;

        return (topleft == TILE_CHEST_DEFAULT || topleft == TILE_CHEST_OPEN ||
                topright == TILE_CHEST_DEFAULT || topright == TILE_CHEST_OPEN ||
                botleft == TILE_CHEST_DEFAULT || botleft == TILE_CHEST_OPEN ||
                botright == TILE_CHEST_DEFAULT || botright == TILE_CHEST_OPEN);
    }

    SDL_Point tile_chest_contact(SDL_Rect* a){
        int x = (a->x + a->w / 2) / tile_width;
        int y = (a->y + a->h / 2) / tile_width;

        // printf("x %d y %d\n", x, y);

        if(tile_type[y][x+1] == TILE_CHEST_DEFAULT) return {x+1, y};
        else if(tile_type[y][x-1] == TILE_CHEST_DEFAULT) return {x-1, y};
        else if(tile_type[y+1][x] == TILE_CHEST_DEFAULT) return {x, y+1};
        else if(tile_type[y-1][x] == TILE_CHEST_DEFAULT) return {x, y-1};
        else if(tile_type[y+1][x+1] == TILE_CHEST_DEFAULT) return {x+1, y+1};
        else if(tile_type[y+1][x-1] == TILE_CHEST_DEFAULT) return {x-1, y+1};
        else if(tile_type[y-1][x+1] == TILE_CHEST_DEFAULT) return {x+1, y-1};
        else if(tile_type[y-1][x-1] == TILE_CHEST_DEFAULT) return {x-1, y-1};
        else return {-1, 0};
    }

    //checks if an entity is colliding with some button
    int tile_button_collission(SDL_Rect* a) {
        if (tile_type[a->y / tile_height][a->x / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[a->y / tile_height][a->x / tile_width] / 100;

        if (tile_type[a->y / tile_height][(a->x + a->w) / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[a->y / tile_height][(a->x + a->w) / tile_width] / 100;

        if (tile_type[(a->y + a->h) / tile_height][a->x / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[(a->y + a->h) / tile_height][a->x / tile_width] / 100;

        if (tile_type[(a->y + a->h) / tile_height][(a->x + a->w) / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[(a->y + a->h) / tile_height][(a->x + a->w) / tile_width] / 100;

        else
            return 0;
    }

    bool tile_endgame_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width];
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width];
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width];
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width];
        return (topleft == 9999 ||
                topright == 9999 ||
                botleft == 9999 ||
                botright == 9999);
    }

    void handle_event(SDL_Event e){
        if(e.user.code == game->event.door_opened){
            
            Mix_PlayChannel(music_channel, door_open_sound, 0);

            game->event.reset(e);

            int door_parent = *(int*)(e.user.data1);

            while (par[door_parent].cnt) {
                par[door_parent].cnt--;
                int gate_row = par[door_parent].gate_row[par[door_parent].cnt];
                int gate_col = par[door_parent].gate_col[par[door_parent].cnt];
                tile_type[gate_row][gate_col] = TILE_GATE_OPEN;
                angle[gate_row][gate_col] = angle[gate_row][gate_col] == 90? 0 : 270;
            }

            int button_row = par[door_parent].button_row;
            int button_col = par[door_parent].button_col;
            tile_type[button_row][button_col] = TILE_BUTTON_TRIGGERED;
        }
    }

    //takes camera as argument. Renders all tiles that fit into the camera. Probably renders 1 tile along all directions outside the camera
    void render(SDL_Rect& camera) {
        int x, y;
        int renderX = 0, renderY = 0;

        //if camera border of the camera is in the middle of some tile, renders the tiles slightly outside the range of the camera
        int translateX = (camera.x) % tile_width, translateY = (camera.y) % tile_height;

        //inerates from left of the camera to right, and top to bottom to find all the tiles that fit within
        for (y = camera.y; y <= (camera.y + camera.h + tile_height); y += tile_height, renderY += tile_height, renderX = 0) {
            for (x = camera.x; x <= (camera.x + camera.w + tile_width); x += tile_width, renderX += tile_width) {
                SDL_Rect dest = {renderX - translateX, renderY - translateY, tile_width, tile_height};

                int row = (int)(y / tile_height);
                int col = (int)(x / tile_width);
                int type = tile_type[row][col] % 100;

                //renders walkable tile under all tiles. so gates and switches seems like they're on walkable tiles
                SDL_RenderCopy(game->renderer, tile_image, &tiles[TILE_WALK0], &dest);

                // float angle = 0.0;

                if(type == TILE_TREE0 || type == TILE_TREE1){
                    short row_add[] = {1, -1, 0, 0};
                    short col_add[] = {0, 0, 1, -1};

                    for(int i=0; i<4; i++) 
                        if(tile_type[row+row_add[i]][col+col_add[i]] >= TILE_WALK0 && tile_type[row+row_add[i]][col+col_add[i]] <= TILE_WALK4){
                            SDL_RenderCopy(game->renderer, tile_image, &tiles[tile_type[row+row_add[i]][col+col_add[i]]], &dest);
                            break;
                        }
                    SDL_RenderCopy(game->renderer, tile_image, &tiles[type], &dest);
                }

                if (type == TILE_WALL0) {
                    SDL_RenderCopyEx(game->renderer, tile_image, &tiles[type], &dest, angle[row][col], NULL, SDL_FLIP_NONE);
                }

                else if(type <= TILE_WALL4){
                    SDL_RenderCopy(game->renderer, tile_image, &tiles[type], &dest);
                }                    

                else if(type <= TILE_WALK4)
                    SDL_RenderCopy(game->renderer, tile_image, &tiles[type], &dest);

                else if(type <= TILE_BUTTON_TRIGGERED){

                    short row_add[] = {1, -1, 0, 0};
                    short col_add[] = {0, 0, 1, -1};

                    for(int i=0; i<4; i++) 
                        if(tile_type[row+row_add[i]][col+col_add[i]] >= TILE_WALK0 && tile_type[row+row_add[i]][col+col_add[i]] <= TILE_WALK4){
                            SDL_RenderCopy(game->renderer, tile_image, &tiles[tile_type[row+row_add[i]][col+col_add[i]]], &dest);
                            break;
                        }

                    SDL_RenderCopy(game->renderer, button_image, &tiles[type], &dest);
                }

                else if (type <= TILE_GATE_OPEN) {
                    dest.w = tiles[type].w;
                    SDL_Point center = {dest.w, dest.h};
                    

                    if(type == TILE_GATE_OPEN) {
                        center = {0, 0};
                        if(tile_type[row][col-1] == type) dest.x += (32-5);
                        else if(tile_type[row-1][col] == type) dest.y += (32-5);
                    }

                    SDL_RenderCopyEx(game->renderer, gate_image, &tiles[type], &dest, angle[row][col], &center, SDL_FLIP_NONE);
                    dest.w = 32;
                    if(type == TILE_GATE_OPEN) dest.x -= (32-5), dest.y -= (32-5);
                }

                else if(type <= TILE_CHEST_OPEN){
                    SDL_RenderCopy(game->renderer, chest_image, &tiles[type], &dest);
                }
            }
        }
    }
};