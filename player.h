#pragma once

#include "game.h"
#include "init.h"
#include "tile.h"

struct Player {
    enum player_status_list {
        PLAYER_IDLE,
        PLAYER_MOVE,
        PLAYER_ATTACK,
        PLAYER_HURT,
        PLAYER_DYING,
        PLAYER_TOTAL_STATI
    };

    enum collectible_reward{
        HEALTH_REGEN,
        BERSERK,
        IMMUNITY,
        SCORE,
        REWARD_TOTAL
    };

    struct Player_stats {
        int hit_points;
        int defence;
        int attack_damage;
        int attack_radius;
        bool immunity;
        int score;
        int xVel;
        int yVel;

        Player_stats() {
            hit_points = 5;
            defence = 10;
            attack_damage = 15;
            immunity = false;
            attack_radius = 24;
            score = 0;
            xVel = 0;
            yVel = 0;
        }
    };

    Tile* tile;
    Game* game;
    Player_stats* stats = new Player_stats;
    
    int frame;                            //the sprite from the spritesheet that will be renderer
    int sprite_per_row[PLAYER_TOTAL_STATI];//walk, attack, idle -each sprite sheet has different numbers of sprites per row and column
    int sprite_per_col[PLAYER_TOTAL_STATI];
    int slow_factor[PLAYER_TOTAL_STATI];  //player being slowed a little
    int player_status;  //knows if the player is idle, walking or attacking
    int reward[REWARD_TOTAL];    
    int music_channel;

    bool dead;  //player can't move if it is in attack animation

    SDL_Texture* look[PLAYER_TOTAL_STATI];  //look is player's texture. differnt spritesheet for walking attacking etc
    SDL_Texture* life_look;
    SDL_Texture* reward_look[REWARD_TOTAL];
    SDL_Rect src;                           //source rectangle from the spritesheet
    SDL_Rect dest;                          //destination rentangle in the map
    SDL_RendererFlip flip;
    SDL_Point player_center;                //center of player's body. need it for line of sight
    Mix_Chunk* walk_sound;

    Player(int w, int h, Game* inpGame, Tile* inpTile) {
        frame = 0;
        dead = false;
        tile = inpTile;
        game = inpGame;
        music_channel = game->sound_channel[game->SFX_CHANNEL_1];

        slow_factor[PLAYER_IDLE] = 7;
        slow_factor[PLAYER_MOVE] = 4;
        slow_factor[PLAYER_ATTACK] = 4;
        slow_factor[PLAYER_HURT] = 4;
        slow_factor[PLAYER_DYING] = 4;

        sprite_per_row[PLAYER_IDLE] = 6;
        sprite_per_col[PLAYER_IDLE] = 2;
        sprite_per_row[PLAYER_MOVE] = 6;
        sprite_per_col[PLAYER_MOVE] = 3;
        sprite_per_row[PLAYER_ATTACK] = 6;
        sprite_per_col[PLAYER_ATTACK] = 2;
        sprite_per_row[PLAYER_HURT] = 6;
        sprite_per_col[PLAYER_HURT] = 2;
        sprite_per_row[PLAYER_DYING] = 5;
        sprite_per_col[PLAYER_DYING] = 3;

        player_status = PLAYER_IDLE;

        dest = {0, 0, w, h};
        src = {0, 0, w, h};

        flip = SDL_FLIP_NONE;
        for(int i=0; i<REWARD_TOTAL; i++) reward[i] = 0;
    }
    
    ~Player() {
        for(int i=0; i<PLAYER_TOTAL_STATI; i++) {
            SDL_DestroyTexture(look[i]);
            look[i] = NULL;
        }

        SDL_DestroyTexture(life_look);
        life_look = NULL;

        for(int i=0; i<REWARD_TOTAL; i++){
            SDL_DestroyTexture(reward_look[i]);
            reward_look[i] = NULL;
        }

        Mix_FreeChunk(walk_sound);
        walk_sound = NULL;

        tile = NULL;
        game = NULL;
        stats = NULL;
    }

    //loading all the player sprites
    void loadFromFile() {

        char anim_path[5][50] = {
            "png/player/animation/idle_player.png",
            "png/player/animation/walk_player.png",
            "png/player/animation/attack_player.png",
            "png/player/animation/hurt_player.png",
            "png/player/animation/dying_player.png"
        };

        char reward_path[4][50] = {
            "png/player/powerup/heart.png",
            "png/player/powerup/berserk.png",
            "png/player/powerup/immunity.png",
            "png/player/powerup/score.png"           
        };
        
        char walk_sound_path[] =  "sound/sfx_step_grass_l.wav";

        char life_path[] = "png/player/life.png";

        for(int i=0; i<5; i++) look[i] = game->texture_loader(anim_path[i]);

        life_look = game->texture_loader(life_path);

        for(int i=0; i<4; i++) reward_look[i] = game->texture_loader(reward_path[i]);

        walk_sound = Mix_LoadWAV(walk_sound_path);
        if(!walk_sound) error_m
    }

    //only used to check if enemy throwable is colliding with platyer
    bool rect_collission(SDL_Rect& rect) {
        if (rect.x + rect.w < dest.x) return 0;
        if (rect.x > dest.x + dest.w) return 0;
        if (rect.y + rect.h < dest.y) return 0;
        if (rect.y > dest.y + dest.h) return 0;
        return 1;
    }

    bool center_rect_collision(SDL_Rect& rectangle) {
        int x = player_center.x;
        int y = player_center.y;
        int r = stats->attack_radius;

        if (player_center.x < rectangle.x)
            x = rectangle.x;
        else if (player_center.x > rectangle.x + rectangle.w)
            x = rectangle.x + rectangle.w;

        if (player_center.y < rectangle.y)
            x = rectangle.y;
        else if (player_center.y > rectangle.y + rectangle.h)
            y = rectangle.y + rectangle.w;

        x = player_center.x - x;
        y = player_center.y - y;

        int distance = x * x + y * y;
        return (distance <= r * r);
    }

    //set where the player will spawn for the level
    void set_spawn_point(int x, int y) {
        dest.x = x * dest.w;
        dest.y = y * dest.h;

        player_center = {x * dest.w + dest.w / 2, y * dest.h + dest.h / 2};
    }

    //moving player based on input.
    void move() {
        
        static int delay = SDL_GetTicks() - 1300;
        if(SDL_GetTicks() - delay > 720) {
            delay = SDL_GetTicks();
            Mix_PlayChannel(music_channel, walk_sound, 0);
        }

        frame++;
        frame %= (sprite_per_row[PLAYER_MOVE] * sprite_per_col[PLAYER_MOVE] * slow_factor[PLAYER_MOVE]);

        dest.x += stats->xVel;
        if (dest.x < 0 || dest.x + dest.w > game->LEVEL_WIDTH || tile->tile_gate_wall_collission(&dest, 7) || tile->tile_chest_collission(&dest, 7)) 
            dest.x -= stats->xVel;

        dest.y += stats->yVel;
        if (dest.y < 0 || dest.y + dest.h > game->LEVEL_HEIGHT || tile->tile_gate_wall_collission(&dest, 7) || tile->tile_chest_collission(&dest, 7)) 
            dest.y -= stats->yVel;

        if(tile->tile_endgame_collission(&dest, 0)) game->level_end = true, stats->score += 100;

        player_center.x = dest.x + dest.w / 2;
        player_center.y = dest.y + dest.h / 2;
    }

    //player will soon have it's stas defined. Work in progress lol
    void get_damaged() {
        frame++;
        if (frame > (sprite_per_row[PLAYER_HURT] * sprite_per_col[PLAYER_HURT] * slow_factor[PLAYER_HURT])) player_status = PLAYER_IDLE, frame = 0;
    }

    void get_rewarded(int& reward_){
        if(reward_ == HEALTH_REGEN) stats->hit_points++;
        else if(reward_ == BERSERK) stats->attack_damage *= 2;
        else if(reward_ == IMMUNITY) stats->immunity = true;
        else if(reward_ == SCORE) stats->score += 120;
    }

    void idle() {
        frame++;
        frame %= (sprite_per_row[PLAYER_IDLE] * sprite_per_col[PLAYER_IDLE] * slow_factor[PLAYER_IDLE]);
    }

    void attack() {
        frame++;
        if (frame >= (sprite_per_row[PLAYER_ATTACK] * sprite_per_col[PLAYER_ATTACK] * slow_factor[PLAYER_ATTACK])) {
            frame = 0;
            player_status = PLAYER_IDLE;
            return;
        }
    }

    void die() {
        frame++;
        if (frame >= (sprite_per_row[PLAYER_DYING] * sprite_per_col[PLAYER_DYING] * slow_factor[PLAYER_DYING])) frame--, dead = true;
    }

    //handles players actions based on human input. Takes argument SDL_Event to check human activity. And tile pointer to check button activity
    void handle_event(SDL_Event& e) {
        if (dead) return;

        if (e.user.code == game->event.player_damaged) {        
            stats->hit_points = stats->hit_points - 1 + stats->immunity;
            if(!stats->immunity) player_status = PLAYER_HURT, frame = 0;
            else stats->immunity = false;
            if (stats->hit_points <= 0) player_status = PLAYER_DYING;
        }

        //player cannot move orbe idle when it's in attack, hurt or death animation
        else if (player_status < PLAYER_ATTACK) {

            const Uint8* key_state = SDL_GetKeyboardState(NULL);

            if(key_state[SDL_SCANCODE_W]) stats->yVel = -6;
            else if (key_state[SDL_SCANCODE_S]) stats->yVel = 6;
            else stats->yVel = 0;

            if (key_state[SDL_SCANCODE_A]) stats->xVel = -6, flip = SDL_FLIP_HORIZONTAL;
            else if (key_state[SDL_SCANCODE_D]) stats->xVel = 6, flip = SDL_FLIP_NONE;
            else stats->xVel = 0;

            if(stats->xVel||stats->yVel){
                if(player_status == PLAYER_IDLE) frame = 0;
                player_status = PLAYER_MOVE;
            } else {
                if(player_status == PLAYER_MOVE) frame = 0;
                player_status = PLAYER_IDLE;
            }

            if (key_state[SDL_SCANCODE_E]) {
                int tile_button_collission = tile->tile_button_collission(&dest);

                if (tile_button_collission) {
                    game->event.create_event(game->event.door_opened, &tile_button_collission, NULL);
                }

                SDL_Point chest_contact = tile->tile_chest_contact(&dest);
                if(chest_contact.x != -1){
                    tile->tile_type[chest_contact.y][chest_contact.x] = tile->TILE_CHEST_OPEN;
                    
                    int reward_ = rand() % REWARD_TOTAL;
                    get_rewarded(reward_);
                    reward[reward_] = 120;
                }
            }

            //mousebutton press makes player attack 
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                player_status = PLAYER_ATTACK, frame = 0;                
                game->event.create_event(game->event.enemy_damaged, NULL, NULL);
            }

        }

        if (player_status == PLAYER_IDLE)
            idle();
        else if (player_status == PLAYER_MOVE)
            move();
        else if (player_status == PLAYER_ATTACK)
            attack();
        else if (player_status == PLAYER_HURT)
            get_damaged();
        else if (player_status == PLAYER_DYING)
            die();
    }

    void heart_render(){
        int temp = stats->hit_points;
        int padding = 32;
        for(SDL_Rect heart_dest = {game->RENDER_WIDTH-24 - padding, padding, 24, 24}; temp; temp--, heart_dest.x -= heart_dest.w)
            SDL_RenderCopy(game->renderer, life_look, NULL, &heart_dest);
    }

    void reward_render(){
        SDL_Rect reward_dest = {0, 0, 32, 42};
        for(int i=0; i<REWARD_TOTAL; i++){
            if(reward[i]){
                reward[i]--;
                SDL_RenderCopy(game->renderer, reward_look[i], NULL, &reward_dest);
                reward_dest.x += 42;
            }
        }
    }

    void render() {
        //slowing player a little
        int current_frame = frame / slow_factor[player_status];

        //centering player on the screen
        dest.x -= game->camera.x;
        dest.y -= game->camera.y;

        //deciding which frame in the spritesheet to render
        src.x = (current_frame % sprite_per_row[player_status]) * src.w;
        src.y = (current_frame / sprite_per_row[player_status]) * src.h;

        SDL_RenderCopyEx(game->renderer, look[player_status], &src, &dest, 0.0, NULL, flip);
        if(stats->hit_points)heart_render();
        reward_render();

        //reverting player to it's original location onthe whole map
        dest.x += game->camera.x;
        dest.y += game->camera.y;
    }
};
