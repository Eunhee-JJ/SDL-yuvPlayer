#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
 
// Display window size
int WINDOW_W = 1280;
int WINDOW_H = 720;
 
int thread_exit = 0;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

FILE *fp;

int refresh_video(void *opaque)
{
    while(thread_exit == 0)
    {
        SDL_Event event;
        event.type = SDL_USEREVENT + 1;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    printf("exit refresh video thread.\n");
    return 0;
}

bool init()
{
    bool success = true;
    
    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else{
        window = SDL_CreateWindow("SDL player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_W, WINDOW_H, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
     
        renderer = SDL_CreateRenderer(window, -1, 0);
     
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, WINDOW_W, WINDOW_H);
    }
    
    return success;
}

bool loadMedia()
{
    bool success = true;
    
    fp = fopen("PeopleOnStreet_1280x720_30_Original.yuv", "rb");
    if (!fp)
    {
        printf("Cannot open the file.\n");
        success = false;
    }
    
    return success;
}

void SDLclose()
{
    //Destory window
    SDL_DestroyWindow(window);
    window = NULL;
    
    //Quit SDL subsystems
    SDL_Quit();
}
 
int main(int argc, char* args[])
{
    if( !init() )
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        //Load Media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else{
            //Main loop flag
            bool quit = false;
            bool play = true;
            
            SDL_Thread *thread = SDL_CreateThread(refresh_video, NULL, NULL);
            SDL_Event event;
            SDL_Rect rect;
            
            // The width and height of the image
            const int IMG_W = 1280;
            const int IMG_H = 720;
            
            // buffer size for YUV420
            uint8_t buff[IMG_W * IMG_H * 3 / 2];
            
            while( !quit )
            {
                while( SDL_PollEvent( &event ) != 0 )
                {
                    if( event.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    else if(event.type == SDL_WINDOWEVENT)
                    {
                        SDL_GetWindowSize(window, &WINDOW_W, &WINDOW_H);
                        printf("window resize, w=%d h=%d\n", WINDOW_W, WINDOW_H);
                    }
                }
                
                // loop for reading frames & update render
                fread(buff, 1, IMG_W * IMG_H * 3 / 2, fp);
                SDL_UpdateTexture(texture, NULL, buff, IMG_W);
     
                rect.x = 0;
                rect.y = 0;
                rect.w = WINDOW_W;
                rect.h = WINDOW_H;
     
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_RenderPresent(renderer);
                
                SDL_Delay(40);
            }
        }
    }
    SDLclose();
    return 0;
}
