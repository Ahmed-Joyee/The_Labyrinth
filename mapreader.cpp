#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
    { printf("sdl error in line %d. Error: %s\n", __LINE__, SDL_GetError()); }
#define error_i \
    { printf("image error in line %d. Error: %s\n", __LINE__, IMG_GetError()); }
#define error_m \
    { printf("mixer error in line %d. Error: %s\n", __LINE__, MIX_GetError()); }

const int WIDTH = 1280;
const int HEIGHT = 960;

void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

void init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) error;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) error_i;

    gWindow = SDL_CreateWindow("main2win", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) error;

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL) error;

    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
}

class LTexture {
   public:
    //Initializes variables
    LTexture();

    //Deallocates memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile(std::string path);

    //Deallocates texture
    void free();

    //Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders texture at given point
    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    //Gets image dimensions
    int getWidth();
    int getHeight();

    //Pixel manipulators
    bool lockTexture();
    bool unlockTexture();
    void* getPixels();
    int getPitch();

   private:
    //The actual hardware texture
    SDL_Texture* mTexture;
    void* mPixels;
    int mPitch;

    //Image dimensions
    int mWidth;
    int mHeight;
};
LTexture::LTexture() {
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
    mPixels = NULL;
    mPitch = 0;
}

LTexture::~LTexture() {
    //Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path) {
    free();

    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(gWindow), 0);
        if (formattedSurface == NULL) {
            printf("Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError());
        } else {
            newTexture = SDL_CreateTexture(gRenderer, SDL_GetWindowPixelFormat(gWindow), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
            if (newTexture == NULL) {
                printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
            } else {
                //Lock texture for manipulation
                SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);

                //Copy loaded/formatted surface pixels
                memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                //Unlock texture to update
                SDL_UnlockTexture(newTexture);
                mPixels = NULL;

                //Get image dimensions
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;
            }

            //Get rid of old formatted surface
            SDL_FreeSurface(formattedSurface);
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::free() {
    //Free texture if it exists
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
        mPixels = NULL;
        mPitch = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
    return mWidth;
}

int LTexture::getHeight() {
    return mHeight;
}

bool LTexture::lockTexture() {
    bool success = true;

    if (mPixels != NULL) {
        printf("Texture is already locked!\n");
        success = false;
    }
    //Lock texture
    else {
        if (SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0) {
            printf("Unable to lock texture! %s\n", SDL_GetError());
            success = false;
        }
    }

    return success;
}

bool LTexture::unlockTexture() {
    bool success = true;

    //Texture is not locked
    if (mPixels == NULL) {
        printf("Texture is not locked!\n");
        success = false;
    }
    //Unlock texture
    else {
        SDL_UnlockTexture(mTexture);
        mPixels = NULL;
        mPitch = 0;
    }

    return success;
}

void* LTexture::getPixels() {
    return mPixels;
}

int LTexture::getPitch() {
    return mPitch;
}

LTexture map;
std::ofstream gFile;

void loadmedia() {
    map.loadFromFile("png/l3.png");
    gFile.open("mapped.txt");
}

int main(int argc, char* argv[]) {
    init();
    loadmedia();

    if (!map.lockTexture()) error;

    SDL_PixelFormat* mappingformat = SDL_AllocFormat(SDL_GetWindowPixelFormat(gWindow));

    Uint32* pixel = (Uint32*)map.getPixels();
    int pixelcount = (map.getPitch() / 4) * map.getHeight();

    const Uint32 wall0 = SDL_MapRGB(mappingformat, 0, 0, 255);
    const Uint32 tree0 = SDL_MapRGB(mappingformat, 64, 255, 64);
    const Uint32 wall2 = SDL_MapRGB(mappingformat, 0, 0, 160);
    const Uint32 tree1 = SDL_MapRGB(mappingformat, 255, 128, 255);
    const Uint32 wall4 = SDL_MapRGB(mappingformat, 0, 170, 250);

    const Uint32 walk0 = SDL_MapRGB(mappingformat, 255, 255, 255);
    const Uint32 walk1 = SDL_MapRGB(mappingformat, 195, 195, 195);
    const Uint32 walk2 = SDL_MapRGB(mappingformat, 64, 128, 64);
    const Uint32 walk3 = SDL_MapRGB(mappingformat, 128, 128, 64);
    const Uint32 walk4 = SDL_MapRGB(mappingformat, 150, 100, 100);

    const Uint32 chest = SDL_MapRGB(mappingformat, 255, 128, 128);

    const Uint32 endgame = SDL_MapRGB(mappingformat, 100, 100, 100);

    Uint8 r, g, b;

    for (int i = 0; i < 120; i++) {
        for (int j = 0; j < 160; j++) {
            SDL_GetRGB(pixel[160 * i + j], mappingformat, &r, &g, &b);

            // gFile << "i" << std::to_string(i) << "j" << std::to_string(j)<<" ";

            if(!b){
                if(!r){
                    if(!(g/10)) gFile << "0";
                    gFile << std::to_string(g) << "12 ";
                } else{
                    if(!(r/10)) gFile << "0";
                    gFile << std::to_string(r) << "10 ";
                }                
            }

            else if (SDL_MapRGB(mappingformat, r, g, b) == wall0)
                gFile << "0000 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == tree0)
                gFile << "0001 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == wall2)
                gFile << "0002 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == tree1)
                gFile << "0003 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == wall4)
                gFile << "0004 ";

            else if (SDL_MapRGB(mappingformat, r, g, b) == walk0)
                gFile << "0005 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == walk1)
                gFile << "0006 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == walk2)
                gFile << "0007 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == walk3)
                gFile << "0008 ";
            else if (SDL_MapRGB(mappingformat, r, g, b) == walk4)
                gFile << "0009 ";

            else if (SDL_MapRGB(mappingformat, r, g, b) == chest)
                gFile << "0014 ";

            else if(SDL_MapRGB(mappingformat, r, g, b) == endgame)
                gFile << "9999 ";
            else gFile << "0005 ";
            
        }
        gFile << "\n";
    }
    SDL_FreeFormat(mappingformat);
    close();
    return 0;
}

void close() {
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    gFile.close();
    SDL_Quit();
    IMG_Quit();
}