#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define error \
    { printf("SDL Error in established %d. Error: %s\n", __LINE__, SDL_GetError()); }
#define error_i \
    { printf("SDL Image Error in established %d. Error: %s\n", __LINE__, IMG_GetError()); }
#define error_t \
    { printf("TTF Error in established %d. Error: %s\n", __LINE__, TTF_GetError()); }
#define error_m \
    { printf("mixer error in established %d. Error: %s\n", __LINE__, Mix_GetError()); }
#define p(x) \
    printf("%s\n", x);
