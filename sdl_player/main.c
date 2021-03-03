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

unsigned int startTime, endTime, FTime;

// The width and height of the image
const int IMG_W = 1280;
const int IMG_H = 720;
const int IMG_F = 30;
int nowFrame = 0;
int nFrame = 0;

// buffer size for YUV420
uint8_t buff[IMG_W * IMG_H * 3 / 2];

bool play = true;
/*
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
*/
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

void displayFrame(SDL_Rect *rect)
{
    // loop for reading frames & update render
    startTime = SDL_GetTicks();
    fread(buff, 1, IMG_W * IMG_H * 3 / 2, fp);
    nowFrame++;
    nFrame++;
    //printf( "Now frame: %d\n", nowFrame );
    SDL_UpdateTexture(texture, NULL, buff, IMG_W);

    rect->x = 0;
    rect->y = 0;
    rect->w = WINDOW_W;
    rect->h = WINDOW_H;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_RenderPresent(renderer);
    
    endTime = SDL_GetTicks();
    FTime = endTime - startTime;
}

void printResult()
{
    printf( "재생 완료. \n" );
    printf( "프레임 당 소요시간 : %dms\n", FTime);
    printf( "총 소요시간: %dms\n", FTime * nFrame );
    printf( "play: %d\n", play );
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
            // Main loop flag
            bool quit = false;
            // Controlling video play flag
            
            //SDL_Thread *thread = SDL_CreateThread(refresh_video, NULL, NULL);
            SDL_Event event;
            SDL_Rect rect;
            
            memset(buff, 0, IMG_W * IMG_H * 3 / 2);
            
            // Main loop
            while( !quit )
            {
                while( SDL_PollEvent( &event ) != 0 )
                {
                    if( event.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    else if( event.type == SDL_KEYDOWN){
                        if( event.key.keysym.sym == SDLK_SPACE ){
                            play = !play;
                            printResult();
                            if( play == true ){
                                printf("Play\n");
                                if( nowFrame == IMG_F ){
                                    printf(" Rewind \n");
                                    rewind( fp );
                                    nowFrame = 0;
                                }
                                nFrame = 0;
                            }
                            else if( play == false ){
                                printf("Pause\n");
                            }
                        }
                        else if( event.key.keysym.sym == SDLK_LEFT ){
                            printf("Previous Frame\n");
                            nowFrame--;
                            nFrame = 1;
                            fseek(fp, IMG_W * IMG_H * 3 / 2 ,SEEK_CUR);
                            displayFrame(&rect);
                            printResult();
                        }
                    }
                    if( play == true && nowFrame < IMG_F ){
                        displayFrame(&rect);
                        
                        // Stop playing at the end of file
                        if(nowFrame == IMG_F){
                            play = !play;
                            printResult();
                        }
                    }
                }
            }
        }
    }
    SDLclose();
    return 0;
}
