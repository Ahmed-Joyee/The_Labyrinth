#include "screen.h"

void UI(Game* game){

    SDL_Texture* logo;
    SDL_Surface* imgTemp = IMG_Load("png/logo.png");
    if(imgTemp == NULL) error_i
    logo = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
    if(logo == NULL) error;

    SDL_Rect logodest = {(game->RENDER_WIDTH - imgTemp->w)/2, 20, imgTemp->w, imgTemp->h};

    SDL_FreeSurface(imgTemp);
    imgTemp = NULL;

    Mix_Music* bg_music = Mix_LoadMUS("sound/Enchanted_Festival.mp3");
    if(!bg_music) error_m
    Mix_FadeInMusic(bg_music, -1, 3000);

    Button* button_start = new Button(game->BUTTON_START, game);
    button_start->loadFromFile("png/buttons/start.png");
    button_start->set_dest(250, 150);

    Button* button_levels = new Button(game->BUTTON_LOAD, game);
    button_levels->loadFromFile("png/buttons/levels.png");
    button_levels->set_dest(250, 225);

    Button* button_options = new Button(game->BUTTON_OPTIONS, game);
    button_options->loadFromFile("png/buttons/options.png");
    button_options->set_dest(250, 300);

    Button* buttont_hiscore = new Button(game->BUTTON_HISCORE, game);
    buttont_hiscore->loadFromFile("png/buttons/hiscore.png");
    buttont_hiscore->set_dest(250, 375);

    Button* button_quit = new Button(game->BUTTON_QUIT, game);
    button_quit->loadFromFile("png/buttons/quit.png");
    button_quit->set_dest(250, 450);

    Background* ui_bg = new Background(game);
    ui_bg->loadFromFile("png/background/layer_0.png", "png/background/layer_1.png", "png/background/layer_2.png");
    ui_bg->set_vel();
    ui_bg->set_rect();
    ui_bg->set_width();

    SDL_Event e;
    while(game->game_running && game->current_screen == game->UI_SCREEN){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);


        button_start->handle_event(e);
        button_levels->handle_event(e);
        button_options->handle_event(e);
        buttont_hiscore->handle_event(e);
        button_quit->handle_event(e);

        ui_bg->render();
        SDL_RenderCopy(game->renderer, logo, NULL, &logodest);

        button_start->render();
        button_levels->render();
        button_options->render();
        buttont_hiscore->render();
        button_quit->render();

        SDL_RenderPresent(game->renderer);
    }

    Mix_FadeOutMusic(500);

    SDL_DestroyTexture(logo);
    logo = NULL;
}

void instructions(Game* game){

    int delay = SDL_GetTicks();

    SDL_Texture* instructions;
    SDL_Rect dest = {(game->RENDER_WIDTH-420)/2, 0, 420, 600};
    SDL_Surface* imgTemp;

    if(game->current_screen == game->INSTRUCTIONS_SCREEN_0){
        imgTemp = IMG_Load("png/instructions/instruction_0.png");
        if(imgTemp == NULL) error_i
    } else if (game->current_screen == game->INSTRUCTIONS_SCREEN_1){
        imgTemp = IMG_Load("png/instructions/instruction_1.png");
        if(imgTemp == NULL) error_i
    } else if (game->current_screen == game->INSTRUCTIONS_SCREEN_2){
        imgTemp = IMG_Load("png/instructions/instruction_2.png");
        if(imgTemp == NULL) error_i
    }

    instructions = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
    if(instructions == NULL) error

    SDL_FreeSurface(imgTemp);
    imgTemp = NULL;

    SDL_Event e;

    while(game->game_running && (game->current_screen >= game->INSTRUCTIONS_SCREEN_0 && game->current_screen <= game->INSTRUCTIONS_SCREEN_2)){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        SDL_RenderCopy(game->renderer, instructions, NULL, &dest);

        if(SDL_GetTicks() - delay > 1000 && (e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONUP)){
            delay = SDL_GetTicks();
            game->current_screen++;

            SDL_Surface* imgTemp;

            if(game->current_screen == game->INSTRUCTIONS_SCREEN_0){
                imgTemp = IMG_Load("png/instructions/instruction_0.png");
                if(imgTemp == NULL) error_i
            } else if (game->current_screen == game->INSTRUCTIONS_SCREEN_1){
                imgTemp = IMG_Load("png/instructions/instruction_1.png");
                if(imgTemp == NULL) error_i
            } else if (game->current_screen == game->INSTRUCTIONS_SCREEN_2){
                imgTemp = IMG_Load("png/instructions/instruction_2.png");
                if(imgTemp == NULL) error_i
            } else break;
            instructions = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
            
            if(instructions == NULL) error

            SDL_FreeSurface(imgTemp);
            imgTemp = NULL;
        };

        SDL_RenderPresent(game->renderer);
    }
}

void levels(Game* game){
    Button* button_home = new Button(game->BUTTON_HOME, game);
    button_home->loadFromFile("png/buttons/home.png");
    button_home->set_dest(50, game->RENDER_HEIGHT-button_home->dest.h-50);

    Button* button_1 = new Button(game->BUTTON_LEVEL_1, game);
    button_1->loadFromFile("png/level/level_1.png");
    button_1->src = {0, 0, 100, 100};
    button_1->dest = {game->RENDER_WIDTH/3 - button_1->src.w / 2, 100, button_1->src.w, button_1->src.h};

    Button* button_2 = new Button(game->BUTTON_LEVEL_2, game);
    button_2->loadFromFile("png/level/level_2.png");
    button_2->src = {0, 0, 100, 100};
    button_2->dest = {(game->RENDER_WIDTH - button_2->src.w) / 2, 100, button_2->src.w, button_2->src.h};

    Button* button_3 = new Button(game->BUTTON_LEVEL_3, game);
    button_3->loadFromFile("png/level/level_3.png");
    button_3->src = {0, 0, 100, 100};
    button_3->dest = {game->RENDER_WIDTH * 2/3 - button_3->src.w / 2, 100, button_3->src.w, button_3->src.h};

    SDL_Event e;

    while(game->game_running && game->current_screen == game->LEVEL_CHOICE){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        button_home->handle_event(e);
        button_1->handle_event(e);
        button_2->handle_event(e);
        button_3->handle_event(e);

        button_home->render();
        button_1->render();
        button_2->render();
        button_3->render();

        SDL_RenderPresent(game->renderer);
    }
}

void options(Game* game){

    SDL_Surface* tmpSurface = IMG_Load("png/options/mid.png");
    if(tmpSurface == NULL) error_i
    SDL_Texture* mid = SDL_CreateTextureFromSurface(game->renderer, tmpSurface);
    if(mid == NULL) error;
    SDL_FreeSurface(tmpSurface);
    tmpSurface = NULL;

    Button* button_home = new Button(game->BUTTON_HOME, game);
    button_home->loadFromFile("png/buttons/home.png");
    button_home->set_dest(50, 50);

    Button* button_instructions = new Button(game->BUTTON_INSTRUCTIONS, game);
    button_instructions->loadFromFile("png/buttons/instructions.png");
    button_instructions->set_dest(50, game->RENDER_HEIGHT-button_home->dest.h-50);

    Button* button_credits = new Button(game->BUTTON_CREDITS, game);
    button_credits->loadFromFile("png/buttons/credits.png");
    button_credits->set_dest(450, game->RENDER_HEIGHT-button_home->dest.h-50);

    Button* music_vol_d = new Button(game->BUTTON_MUSIC_VOL_DOWN, game);
    music_vol_d->loadFromFile("png/options/left.png");
    music_vol_d->src = {0, 0, 36, 100};
    music_vol_d->dest = {game->RENDER_WIDTH/2, 166, 36, 100};

    Button* music_vol_i = new Button(game->BUTTON_MUSIC_VOL_UP, game);
    music_vol_i->loadFromFile("png/options/right.png");
    music_vol_i->src = {0, 0, 36, 100};
    music_vol_i->dest = {game->RENDER_WIDTH-music_vol_i->src.w, 166, 36, 100};

    Button* sfx_vol_d = new Button(game->BUTTON_SFX_VOL_DOWN, game);
    sfx_vol_d->loadFromFile("png/options/left.png");
    sfx_vol_d->src = {0, 0, 36, 100};
    sfx_vol_d->dest = {game->RENDER_WIDTH/2, 270, 36, 100};

    Button* sfx_vol_i = new Button(game->BUTTON_SFX_VOL_UP, game);
    sfx_vol_i->loadFromFile("png/options/right.png");
    sfx_vol_i->src = {0, 0, 36, 100};
    sfx_vol_i->dest = {game->RENDER_WIDTH-sfx_vol_i->src.w, 270, 36, 100};

    Button* toggle_window_left = new Button(game->BUTTON_TOGGLE_WINDOW_MODE, game);
    toggle_window_left->loadFromFile("png/options/left.png");
    toggle_window_left->src = {0, 0, 36, 100};
    toggle_window_left->dest = {game->RENDER_WIDTH/2, 380, 36, 100};

    Button* toggle_window_right = new Button(game->BUTTON_TOGGLE_WINDOW_MODE, game);
    toggle_window_right->loadFromFile("png/options/right.png");
    toggle_window_right->src = {0, 0, 36, 100};
    toggle_window_right->dest = {game->RENDER_WIDTH-toggle_window_right->src.w, 380, 36, 100};

    SDL_Rect music_vol_dest = {50, 166, 310, 100};
    SDL_Rect sfx_vol_dest = {50, 270, 310, 100};
    SDL_Rect mid_dest = {0, 0, 22, 100};
    SDL_Rect window_mode_out_dest = {50, 380, 310, 100};
    SDL_Rect window_mode_in_dest = {toggle_window_left->dest.x + toggle_window_left->dest.w + 100, toggle_window_left->dest.y + 21, 0, 0};

    SDL_Event e;

    while(game->game_running && game->current_screen == game->OPTIONS_SCREEN){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        button_home->handle_event(e);
        button_instructions->handle_event(e);
        button_credits->handle_event(e);
        music_vol_d->handle_event(e);
        music_vol_i->handle_event(e);
        sfx_vol_d->handle_event(e);
        sfx_vol_i->handle_event(e);
        toggle_window_left->handle_event(e);
        toggle_window_right->handle_event(e);

        SDL_RenderCopy(game->renderer, game->music_volume, NULL, &music_vol_dest);
        SDL_RenderCopy(game->renderer, game->sfx_volume, NULL, &sfx_vol_dest);
        SDL_RenderCopy(game->renderer, game->window_mode, NULL, &window_mode_out_dest);

        button_home->render();

        music_vol_d->render();
        mid_dest = {music_vol_d->dest.x+music_vol_d->dest.w + 11, music_vol_d->dest.y + 12, 22, 76};
        for(int i=0; i<9; i++, mid_dest.x +=( mid_dest.w + 11)) {
            if(game->sound_level[game->MUSIC_CHANNEL] / 16 == i){
                mid_dest.y -= 12;
                mid_dest.h = 100;
                SDL_RenderCopy(game->renderer, mid, NULL, &mid_dest);
                mid_dest.y += 12;
                mid_dest.h = 76;
            }
            else SDL_RenderCopy(game->renderer, mid, NULL, &mid_dest);
        }
        music_vol_i->render();

        sfx_vol_d->render();
        mid_dest = {sfx_vol_d->dest.x+sfx_vol_d->dest.w + 11, sfx_vol_d->dest.y + 12, 22, 76};
        for(int i=0; i<9; i++, mid_dest.x +=( mid_dest.w + 11)) {
            if(game->sound_level[game->SFX_CHANNEL_1] / 16 == i){
                mid_dest.y -= 12;
                mid_dest.h = 100;
                SDL_RenderCopy(game->renderer, mid, NULL, &mid_dest);
                mid_dest.y += 12;
                mid_dest.h = 76;
            }
            else SDL_RenderCopy(game->renderer, mid, NULL, &mid_dest);
        }
        sfx_vol_i->render();

        toggle_window_left->render();
        if (game->fullscreen){
            SDL_QueryTexture(game->window_fullscreen, NULL, NULL, &window_mode_in_dest.w, &window_mode_in_dest.h);
            SDL_RenderCopy(game->renderer, game->window_fullscreen, NULL, &window_mode_in_dest);
        } else {
            SDL_QueryTexture(game->window_windowed, NULL, NULL, &window_mode_in_dest.w, &window_mode_in_dest.h);
            SDL_RenderCopy(game->renderer, game->window_windowed, NULL, &window_mode_in_dest);
        }
        toggle_window_right->render();

        button_instructions->render();
        button_credits->render();

        SDL_RenderPresent(game->renderer);
    }

}

void hiscore(Game* game){

    game->read_score();

    Button* button_home = new Button(game->BUTTON_HOME, game);
    button_home->loadFromFile("png/buttons/home.png");
    button_home->set_dest(50, game->RENDER_HEIGHT-button_home->dest.h-50);

    SDL_Surface* imgTemp = NULL;
    SDL_Texture* level_text[game->LEVELTOTAL];
    SDL_Texture* score_text[game->LEVELTOTAL];
    for(int i=0; i<game->LEVELTOTAL; i++){
        std::string lv = "LEVEL " + std::to_string(i+1);
        imgTemp = TTF_RenderText_Solid(game->font, lv.c_str(), {227, 150, 62});
        if(imgTemp == NULL) error_t
        level_text[i] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(level_text[i] == NULL) error        

        imgTemp = TTF_RenderText_Solid(game->font, std::to_string(game->score[i]).c_str(), {227, 150, 62});
        if(imgTemp == NULL) error_t
        score_text[i] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(score_text[i] == NULL) error
    }
    SDL_FreeSurface(imgTemp);
    imgTemp = NULL;

    SDL_Event e;

    while(game->game_running && game->current_screen == game->HISCORE_SCREEN){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        button_home->handle_event(e);
        
        for(int i=0; i<game->LEVELTOTAL; i++){
            game->text_render(level_text[i], 20, 75 * (i+1));
            game->text_render(score_text[i], game->RENDER_WIDTH/2 + 20, 75 * (i+1));
        }
        button_home->render();

        SDL_RenderPresent(game->renderer);
    }
}

void credits(Game* game){

    int line_spacing = 36;
    int paragraph_spacing = 42;

    SDL_Texture* textwall[7];
    SDL_Rect dest[7];

    char text[7][35] = {"A WORK OF", "MAHDI MOHAMMED HOSSAIN NOKI", "ROLL AE 02", "MOHIMA AHMED JOYEE", "ROLL SK 42", "COMPUTER SCIENCE AND ENGINEERING", "UNIVERSITY OF DHAKA"};

    SDL_Surface* imgTemp = NULL;
    int w, h;

    for(int i=0; i<7; i++){
        imgTemp = TTF_RenderText_Blended(game->font, text[i], {227, 150, 62});
        if(imgTemp == NULL) error_t
        textwall[i] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(textwall[i] == NULL) error

        SDL_QueryTexture(textwall[i], NULL, NULL, &w, &h);
        dest[i] = {(game->RENDER_WIDTH-w)/2, i==0?game->RENDER_HEIGHT:dest[i-1].y+dest[i-1].h+paragraph_spacing, w, h};
        if(text[i][0] == 'R' || text[i][0] == 'U') dest[i].y = dest[i-1].y + line_spacing;
    }
    SDL_FreeSurface(imgTemp);
    imgTemp = NULL;

    SDL_Event e;

    while(game->game_running && game->current_screen == game->CREDITS_SCREEN){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;
        else if(e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONUP) game->current_screen = game->UI_SCREEN;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        for(int i=0; i<7; i++) SDL_RenderCopy(game->renderer, textwall[i], NULL, &dest[i]);

        for(int i=0; dest[0].y >= 74 && i<7; i++) dest[i].y--;

        SDL_RenderPresent(game->renderer);
    }

}

void pause_screen(Game* game, int score_){

    Button* button_home = new Button(game->BUTTON_HOME, game);
    button_home->loadFromFile("png/buttons/home.png");
    button_home->set_dest(50, 200);

    Button* button_continue = new Button(game->BUTTON_CONTINUE, game);
    button_continue->loadFromFile("png/buttons/continue.png");
    button_continue->set_dest(450, 200);

    Score* score = new Score(game, score_);
    score->set_height(100);

    SDL_Event e;

    while(game->game_running && game->game_pause){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

        button_home->handle_event(e);
        button_continue->handle_event(e);

        score->render();
        button_home->render();
        button_continue->render();

        SDL_RenderPresent(game->renderer);
    }

}

void level_end_screen(Game* game, Player* player_, int on_level){
    
    if(player_->stats->score > game->score[on_level - game->LEVEL_1]){
        game->score[on_level - game->LEVEL_1] = player_->stats->score;
        game->write_score();
    }

    Button* button_home = new Button(game->BUTTON_HOME, game);
    button_home->loadFromFile("png/buttons/home.png");
    button_home->set_dest(50, 450);

    Button* button_next = new Button(game->BUTTON_NEXT, game);
    button_next->loadFromFile("png/buttons/continue.png");
    button_next->set_dest(450, 450);

    if(player_->dead) button_home->set_dest(250, 250);

    Score* score = new Score(game, player_->stats->score);
    score->set_height(100);

    SDL_Event e;

    while(game->game_running && game->current_screen == on_level){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        
        button_home->handle_event(e);
        if(!player_->dead) button_next->handle_event(e);

        score->render_countup();
        button_home->render();
        if(!player_->dead) button_next->render();

        SDL_RenderPresent(game->renderer);
    }
}

void loadTile(Tile* tile, std::string file_path, int level_w, int level_h){
    tile->loadImageFromFile();
    tile->loadInfoFromFile(file_path, level_w, level_h);
}

void loadPlayer(Player* player){
    player->loadFromFile();
}

void loadEnemy(Enemy* enemy_one){
    enemy_one->loadFromFile();
}

void level_one(Game* game){
    game->level_end = false;
    game->set_level_dimension(1280, 960);

    Mix_Music* lv_music = Mix_LoadMUS("sound/Woodland_Fantasy.mp3");
    if(!lv_music) error_m
    Mix_FadeInMusic(lv_music, -1, 1000);

    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "map/level_1.txt", 40, 30);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(34, 4);
    
    Enemy* enemy_one = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_one);
    enemy_one->set_spawn(19, 21, enemy_one->LEFT_RIGHT); 

    Enemy* enemy_two = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_two);
    enemy_two->set_spawn(8, 8, enemy_two->UP_DOWN);   

    SDL_Event e;

    while(game->game_running && game->current_screen == game->LEVEL_1){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if(key[SDL_SCANCODE_ESCAPE]) game->game_pause = true;

        if(game->game_pause) pause_screen(game, player->stats->score);

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy_one->handle_event(e);
        enemy_two->handle_event(e);
        player->handle_event(e);
        tile->handle_event(e);
        game->event.reset(e);

        if(game->level_end) level_end_screen(game, player, game->current_screen);
        if(player->dead) level_end_screen(game, player, game->current_screen);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        enemy_one->render(game->camera);
        enemy_two->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }

    Mix_FadeOutMusic(500);
}

void level_two(Game* game){
    game->level_end = false;
    game->set_level_dimension(2560, 1920);

    Mix_Music* lv_music = Mix_LoadMUS("sound/Woodland_Fantasy.mp3");
    if(!lv_music) error_m
    Mix_FadeInMusic(lv_music, -1, 1000);

    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "map/level_2.txt", 80, 60);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(76, 3);


    Enemy* enemy_one = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_one);
    enemy_one->set_spawn(67, 16, enemy_one->UP_DOWN);   
    
    Enemy* enemy_two = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_two);
    enemy_two->set_spawn(34, 29, enemy_two->LEFT_RIGHT);  

    Enemy* enemy_three = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_three);
    enemy_three->set_spawn(35, 43, enemy_three->UP_DOWN);   

    Enemy* enemy_four = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_four);
    enemy_four->set_spawn(49, 53, enemy_four->UP_DOWN);   

    SDL_Event e;

    while(game->game_running && game->current_screen == game->LEVEL_2){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if(key[SDL_SCANCODE_ESCAPE])game->game_pause = true;

        if(game->game_pause) pause_screen(game, player->stats->score);

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy_one->handle_event(e);
        enemy_two->handle_event(e);
        enemy_three->handle_event(e);
        enemy_four->handle_event(e);

        player->handle_event(e);
        tile->handle_event(e);
        game->event.reset(e);

        if(game->level_end) level_end_screen(game, player, game->current_screen);
        if(player->dead) level_end_screen(game, player, game->current_screen);

        game->camera_set(&player->dest);

        tile->render(game->camera);

        enemy_one->render(game->camera);
        enemy_two->render(game->camera);
        enemy_three->render(game->camera);
        enemy_four->render(game->camera);
        
        player->render();

        SDL_RenderPresent(game->renderer);
    }
    Mix_FadeOutMusic(500);

}

void level_three(Game* game){
    game->level_end = false;
    game->set_level_dimension(5120, 3840);

    Mix_Music* lv_music = Mix_LoadMUS("sound/Woodland_Fantasy.mp3");
    if(!lv_music) error_m
    Mix_FadeInMusic(lv_music, -1, 1000);

    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "map/level_3.txt", 160, 120);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(134, 19);

    Enemy* enemy_one = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_one);
    enemy_one->set_spawn(111, 30, enemy_one->UP_DOWN);  

    Enemy* enemy_two = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_two);
    enemy_two->set_spawn(122, 69, enemy_two->LEFT_RIGHT);  

    Enemy* enemy_three = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_three);
    enemy_three->set_spawn(48, 91, enemy_three->LEFT_RIGHT);  

    Enemy* enemy_four = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_four);
    enemy_four->set_spawn(67, 45, enemy_four->UP_DOWN);  

    Enemy* enemy_five = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_five);
    enemy_five->set_spawn(25, 40, enemy_five->LEFT_RIGHT);  

    SDL_Event e;

    while(game->game_running && game->current_screen == game->LEVEL_3){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if(key[SDL_SCANCODE_ESCAPE])game->game_pause = true;

        if(game->game_pause) pause_screen(game, player->stats->score);

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy_one->handle_event(e);
        enemy_two->handle_event(e);
        enemy_three->handle_event(e);
        enemy_four->handle_event(e);
        enemy_five->handle_event(e);

        player->handle_event(e);
        tile->handle_event(e);
        game->event.reset(e);

        if(game->level_end) level_end_screen(game, player, game->current_screen);
        if(player->dead) level_end_screen(game, player, game->current_screen);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        enemy_one->render(game->camera);
        enemy_two->render(game->camera);
        enemy_three->render(game->camera);
        enemy_four->render(game->camera);
        enemy_five->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }
    Mix_FadeOutMusic(500);
}