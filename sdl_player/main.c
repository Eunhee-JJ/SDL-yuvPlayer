//
//  main.c
//  sdl_player
//
//  Created by 정은희 on 2021/02/27.
//

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

int main(int argc, const char * argv[]) {
    static int display_in_use = 0; /* Only using first display */

    int i, display_mode_count;
    SDL_DisplayMode mode;
    Uint32 f;
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Log("SDL_GetNumVideoDisplays(): %i", SDL_GetNumVideoDisplays());

    display_mode_count = SDL_GetNumDisplayModes(display_in_use);
    if (display_mode_count < 1) {
        SDL_Log("SDL_GetNumDisplayModes failed: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("SDL_GetNumDisplayModes: %i", display_mode_count);

    for (i = 0; i < display_mode_count; ++i) {
        if (SDL_GetDisplayMode(display_in_use, i, &mode) != 0) {
            SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
            return 1;
        }
        f = mode.format;

        SDL_Log("Mode %i\tbpp %i\t%s\t%i x %i", i,
        SDL_BITSPERPIXEL(f), SDL_GetPixelFormatName(f), mode.w, mode.h);
    }
    return 0;
}
