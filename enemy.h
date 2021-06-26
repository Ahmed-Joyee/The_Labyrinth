#pragma once

#include "game.h"
#include "init.h"
#include "player.h"
#include "tile.h"

struct Enemy {
    enum enemy_status_list {
        ENEMY_PATROL,
        ENEMY_ATTACK,
        ENEMY_HURT,
        ENEMY_DYING,
        ENEMY_TOTAL
    };

    enum route {
        UP_DOWN,
        LEFT_RIGHT
    };

    struct Stats{
        int xVel;
        int yVel;
        int vel;
        int hit_point;
        int hit_point_max;
        int route_length;
        int attack_range;
        Uint32 attack_delay;

        Stats(){
            vel = 2;
            hit_point = 35;
            hit_point_max = 35;
            attack_range = 180;
            attack_delay = 2700;
            route_length = 120;
        }
    };

    struct Projectile{ 
        bool launched;
        int vel;
        int xVel;
        int yVel;
        int angle;

        Projectile(){
            launched = false;
            vel = 2;
            xVel = 0.0;
            yVel = 0.0;
            angle = 0.0;
        }
    }; 

    Game* game;
    Player* player;
    Tile* tile;

    //using bresenham's line drawing algorithm to find a straight line between player and enemy
    struct LineOfSight {
        int dx;  //change in x coordinate from enemy to player
        int dy;  //change in y coordinate from enemy to player
        int range;

        //starting point and endingpoint of the line
        SDL_Point start;
        SDL_Point end;

        Tile* tile;

        LineOfSight() {
            tile = NULL;
        }

        //checking if player is in range of enemy
        bool in_range() {
            return dx * dx + dy * dy <= range;
        }

        //resetting enemy position and player position
        void reset(SDL_Point* player_center, SDL_Point* enemy_center) {
            start = *player_center;
            end = *enemy_center;

            dx = end.x - start.x;
            dy = end.y - start.y;
        }

        bool low_slope(SDL_Point& start, SDL_Point& end) {
            if (dx < 0) {
                dx = -dx;
                dy = -dy;
                return low_slope(end, start);
            }

            int y_increment = 1;
            int x_increment = 1;
            if (dy < 0) y_increment = -y_increment, dy = -dy;

            int decision_factor = 2 * dy - dx;

            while (start.x < end.x) {
                start.x += x_increment;
                if (decision_factor > 0)
                    start.y += y_increment, decision_factor += (2 * (dy - dx));
                else
                    decision_factor += (2 * dy);

                int tileType = tile->tile_type[start.y / tile->tile_height][start.x / tile->tile_width];
                if ((tileType <= tile->TILE_WALL2)) return 0;
            }
            return 1;
        }

        bool hi_slope(SDL_Point& start, SDL_Point& end) {
            if (dy < 0) {
                dx = -dx;
                dy = -dy;
                return hi_slope(end, start);
            }
            int x_increment = 1;
            int y_increment = 1;
            if (dx < 0) x_increment = -x_increment, dx = -dx;
            int decision_factor = 2 * dx - dy;

            while (start.y < end.y) {
                start.y += y_increment;

                if (decision_factor > 0)
                    start.x += x_increment, decision_factor += (2 * (dx - dy));
                else
                    decision_factor += (2 * dx);

                int tileType = tile->tile_type[start.y / tile->tile_height][start.x / tile->tile_width];
                if ((tileType <= tile->TILE_WALL2)) return 0;
            }
            return 1;
        }

        //returns 1 if it could establish an uninturrepted line between enemy and player
        bool established() {
            if (dx * dx + dy * dy > range) return 0;
            if (abs(dy) > abs(dx))
                return hi_slope(start, end);
            else
                return low_slope(start, end);
        }
    };

    int originX;  //origin where enemy was spawned. Enemy can move a little about this point
    int originY;
    int enemy_status;                  //whether enemy is attacking
    int frame;                         //frame of enemy to be renderer
    int slow_factor[ENEMY_TOTAL];      //slowing enemy a little
    int sprites_per_row[ENEMY_TOTAL];  //enemy has different amount of sprites in each of it's spritesheet
    int sprites_per_col[ENEMY_TOTAL];

    int music_channel;
    
    bool dead;

    SDL_Texture* look[ENEMY_TOTAL];  //different textures for different enemy status. attack patrol etc.
    SDL_Texture* projectile_look;         //texture for projectile. Enemy launches it's weapon
    SDL_Texture* health_bar;
    SDL_Rect src;                    //source rectangle from the spritesheer
    SDL_Rect dest;                   //destination rectangle in the map
    SDL_Rect projectile_dest;        //destination rectangle in the map for projectile
    SDL_Rect health_rect;
    SDL_RendererFlip flip;           //enemy faces left when it's going/attacking to the left
    SDL_Point enemy_center;          //enemy's body center. Used for establishing line of sight
    SDL_Point projectile_center;     //Center of projectile. Obsolete. Don't delete
    Sint32 player_damage_event_code;

    Mix_Chunk* death_sound;
    Mix_Chunk* shoot_sound ;

    LineOfSight line_of_sight;
    Stats* stats = new Stats;
    Projectile* projectile = new Projectile;

    Enemy(int w, int h, Game* inpGame, Tile* inpTile, Player* inpPlayer) {
        player = inpPlayer;
        tile = inpTile;
        line_of_sight.tile = inpTile;
        game = inpGame;

        music_channel = game->sound_channel[game->SFX_CHANNEL_2];

        src = {0, 0, w, h};
        dest = {0, 0, w, h};
        projectile_dest = {0, 0, 24, 8};
        player_damage_event_code = 69;

        dead = false;
        frame = 0;
        slow_factor[ENEMY_PATROL] = 6;
        slow_factor[ENEMY_ATTACK] = 6;
        slow_factor[ENEMY_HURT] = 4;
        slow_factor[ENEMY_DYING] = 3;

        sprites_per_row[ENEMY_PATROL] = 6;
        sprites_per_col[ENEMY_PATROL] = 3;
        sprites_per_row[ENEMY_ATTACK] = 6;
        sprites_per_col[ENEMY_ATTACK] = 2;
        sprites_per_row[ENEMY_HURT] = 6;
        sprites_per_col[ENEMY_HURT] = 2;
        sprites_per_row[ENEMY_DYING] = 5;
        sprites_per_col[ENEMY_DYING] = 3;        

        line_of_sight.range = stats->attack_range * stats->attack_range;
        enemy_status = ENEMY_PATROL;
        flip = SDL_FLIP_NONE;
        for (int i = 0; i < ENEMY_TOTAL; i++) look[i] = NULL;
    }

    ~Enemy() {
        for (int i = 0; i < ENEMY_TOTAL; i++) {
            SDL_DestroyTexture(look[i]);
            look[i] = NULL;
        }
    }

    //loading all the textures for enemy
    void loadFromFile() {

        char anim_path[4][50] = {
            "png/enemy/animation/walk_enemy.png",
            "png/enemy/animation/attack_enemy.png",
            "png/enemy/animation/hurt_enemy.png",
            "png/enemy/animation/dying_enemy.png"
        };

        char weapon_path[] = "png/enemy/weapon_enemy.png";
        char health_bar_path[] = "png/enemy/health_bar.png";

        char death_sound_path[] = "sound/Death.wav";
        char shoot_sound_path[] = "sound/Crossbow_Shot.wav";

        for(int i=0; i<4; i++) look[i] = game->texture_loader(anim_path[i]);

        projectile_look = game->texture_loader(weapon_path);
        health_bar = game->texture_loader(health_bar_path);

        death_sound = Mix_LoadWAV(death_sound_path);
        if(!death_sound) error_m

        shoot_sound = Mix_LoadWAV(shoot_sound_path);
        if(!shoot_sound) error_m
    }

    //setting player spawn point
    void set_spawn(int x, int y, bool route) {
        originX = x * dest.w;
        originY = y * dest.h;

        enemy_center = {x * dest.w + dest.w / 2, y * dest.h + dest.h / 2};

        dest.x = originX;
        dest.y = originY;
        
        stats->xVel = route * stats->vel;
        stats->yVel = !route * stats->vel;
    }

    bool circle_rect_collision(SDL_Point* center, int* radius, SDL_Rect* rectangle) {
        int x = center->x;
        int y = center->y;
        int r = *radius;

        if (center->x < rectangle->x)
            x = rectangle->x;
        else if (center->x > rectangle->x + rectangle->w)
            x = rectangle->x + rectangle->w;

        if (center->y < rectangle->y)
            x = rectangle->y;
        else if (center->y > rectangle->y + rectangle->h)
            y = rectangle->y + rectangle->w;

        x = center->x - x;
        y = center->y - y;

        int distance = x * x + y * y;
        return (distance <= r * r);
    }

    //moving projectile when enemy is in move stance
    void move() {
        dest.x += stats->xVel;
        if (dest.x < 0 || dest.x + dest.w > game->LEVEL_WIDTH || tile->tile_gate_wall_collission(&dest, 7) || dest.x == originX + stats->route_length || dest.x == originX)
            stats->xVel = -stats->xVel;

        dest.y += stats->yVel;
        if (dest.y < 0 || dest.y + dest.h > game->LEVEL_HEIGHT || tile->tile_gate_wall_collission(&dest, 7) || dest.y == originY + stats->route_length || dest.y == originY)
            stats->yVel = -stats->yVel;

        enemy_center = {dest.x + dest.w / 2, dest.y + dest.h / 2};

        if (stats->xVel > 0) flip = SDL_FLIP_NONE;
        if (stats->xVel < 0) flip = SDL_FLIP_HORIZONTAL;
        frame++;
        frame %= (sprites_per_row[enemy_status] * sprites_per_col[enemy_status] * slow_factor[enemy_status]);

        //of projectile is not launched, projectile is on the enemy body
        if (!projectile->launched)
            projectile_dest.x = dest.x,
            projectile_dest.y = dest.y;
    }

    //makes projectile leave enemy body and go for the player. Takes player's pointer as argument
    void launch_projectile() {

        Mix_PlayChannel(music_channel, shoot_sound, 0);

        projectile->launched = true;
        projectile_center = enemy_center;
        const int dy = player->player_center.y - enemy_center.y;
        const int dx = player->player_center.x - enemy_center.x;
        double component_factor = atan((double)abs(dy) / (double)abs(dx));

        projectile->xVel = (dx > 0 ? 1.0 : -1.0) * (double)projectile->vel * cos(component_factor);
        projectile->yVel = (dy > 0 ? 1.0 : -1.0) * (double)projectile->vel * sin(component_factor);
    }

    //once projectile is launched, moves projectile towards plater. Takes tile pointer as argument to check wall collission and player pointer to check player collsiion
    void move_projectile() {
        static double projectile_xPos = 0;
        static double projectile_yPos = 0;

        projectile_center.x -= (int)projectile_xPos;
        projectile_xPos += projectile->xVel;

        projectile_center.y -= (int)projectile_yPos;
        projectile_yPos += projectile->yVel;

        projectile->angle += 20;
        if (projectile->angle >= 360.0) projectile->angle = 0;

        //if projectile has already travelled it's attack distance, it teleports back to enemybody
        if ((int)projectile_xPos * (int)projectile_xPos + (int)projectile_yPos * (int)projectile_yPos > stats->attack_range * stats->attack_range) {
            projectile->launched = false;
            projectile_xPos = 0;
            projectile_yPos = 0;
            return;
        }

        projectile_center.x += (int)projectile_xPos;
        projectile_center.y += (int)projectile_yPos;
        projectile_dest.x = projectile_center.x - projectile_dest.w / 2;
        projectile_dest.y = projectile_center.y - projectile_dest.h / 2;

        //if projectile collides with player, player will soon get damaged.
        if (player->rect_collission(projectile_dest)) {
            
            game->event.create_event(game->event.player_damaged, NULL, NULL);

            projectile->launched = false;
            projectile_xPos = 0;
            projectile_yPos = 0;
            return;
        }
        //if projectile collides with wall, it teleports back to enemy body
        else if (tile->tile_wall_collission(&projectile_dest, 0)) {
            projectile->launched = false;
            projectile_xPos = 0;
            projectile_yPos = 0;
            return;
        }
    }

    //player attack stance
    void attack() {
        static Uint32 start = SDL_GetTicks() - stats->attack_delay;

        //launches a projectile after attack delay
        if (SDL_GetTicks() - start > stats->attack_delay) launch_projectile(), start = SDL_GetTicks(), frame = 0;

        if (frame >= 0) frame++;
        if (frame >= (sprites_per_row[enemy_status] * sprites_per_col[enemy_status] * slow_factor[enemy_status])) frame = -1;
    }

    void get_damaged() {
        frame++;
        if (frame >= (sprites_per_row[ENEMY_HURT] * sprites_per_col[ENEMY_HURT] * slow_factor[ENEMY_HURT])) enemy_status = ENEMY_PATROL, frame = 0;
    }

    void die() {
        frame++;
        if (frame >= (sprites_per_row[ENEMY_DYING] * sprites_per_col[ENEMY_DYING] * slow_factor[ENEMY_DYING]))  
            dead = true, frame--, player->stats->score += 40;;
    }

    //enemy responding to player events.
    void handle_event(SDL_Event& e) {
        if(projectile->launched) move_projectile();
        if (dead) return;

        //resetting points to establish line of sight each time player or enemy moves
        line_of_sight.reset(&player->player_center, &enemy_center);
        
        if (e.user.code == game->event.enemy_damaged) {

            SDL_Rect attack_rect;
            if (player->flip == SDL_FLIP_NONE)
                attack_rect = {player->dest.x + player->dest.w / 2, player->dest.y - player->dest.h / 2, player->dest.w, player->dest.h};
            else if (player->flip == SDL_FLIP_HORIZONTAL)
                attack_rect = {player->dest.x - player->dest.w / 2, player->dest.y - player->dest.h / 2, player->dest.w, player->dest.h};

            if (game->collision(&attack_rect, &dest)) {

                enemy_status = ENEMY_HURT;
                player->stats->score += 15;
                frame = 0;
                stats->hit_point -= player->stats->attack_damage;

                if (stats->hit_point <= 0) {
                    enemy_status = ENEMY_DYING, frame = 0;
                    Mix_PlayChannel(music_channel, death_sound, 0);
                }
            }
        }
        
        if(enemy_status < ENEMY_HURT) {
            if (enemy_status < ENEMY_HURT && line_of_sight.established() && !player->dead) {
                if (enemy_status == ENEMY_PATROL) frame = 0;

                flip = enemy_center.x - player->player_center.x < 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

                enemy_status = ENEMY_ATTACK;
            } else {  //else continues patrolling it's route
                if (enemy_status == ENEMY_ATTACK) frame = 0;
                enemy_status = ENEMY_PATROL;
            }
        }
        
        if(player->dead) enemy_status = ENEMY_PATROL;

        if (enemy_status == ENEMY_PATROL)
            move();
        else if (enemy_status == ENEMY_ATTACK)
            attack();
        else if (enemy_status == ENEMY_HURT)
            get_damaged();
        else if (enemy_status == ENEMY_DYING)
            die();
    }

    //rendering enemy based on camera. Enemy will always be rendered no matter where camera is;
    void render(SDL_Rect& camera) {

        int current_frame = frame / slow_factor[enemy_status];
        src.x = (current_frame % sprites_per_row[enemy_status]) * src.w;
        src.y = (current_frame / sprites_per_row[enemy_status]) * src.h;

        int translateX = camera.x;
        int translateY = camera.y;

        //rendering enemy it it's own patrol route, even if not in camera range
        dest.x -= translateX;
        dest.y -= translateY;
        projectile_dest.x -= translateX;
        projectile_dest.y -= translateY;

        int health_bar_width = (double)stats->hit_point/(double)stats->hit_point_max * (double)dest.w;

        health_rect = {dest.x, dest.y - 12, health_bar_width, 4};

        SDL_RenderCopyEx(game->renderer, look[enemy_status], &src, &dest, 0.0, NULL, flip);
        SDL_RenderCopy(game->renderer, health_bar, NULL, &health_rect);
        if (projectile->launched)  //if projectile is launched, renders projectile
            SDL_RenderCopyEx(game->renderer, projectile_look, NULL, &projectile_dest, projectile->angle, NULL, SDL_FLIP_NONE);

        dest.x += translateX;
        dest.y += translateY;
        projectile_dest.x += translateX;
        projectile_dest.y += translateY;
    }
};