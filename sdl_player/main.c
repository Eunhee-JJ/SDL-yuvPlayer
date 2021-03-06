#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
 
// Display window size
int WINDOW_W = 1280;
int WINDOW_H = 720;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

// YUV video file
FILE *fp;

// For counting the time required
unsigned int startTime, endTime, FTime, sumFTime = 0;
int nowFrame = 0;
int nFrame = 0;

// The width and height of the image
const int IMG_W = 1280;
const int IMG_H = 720;
const int IMG_F = 30;

// Buffer for YUV420
uint8_t buff[IMG_W * IMG_H * 3 / 2];

// Flag for play/pause
bool play = true;

// SDL init
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

// Load YUV file
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

// Display a frame
void displayFrame(SDL_Rect *rect)
{
    startTime = SDL_GetTicks(); // YUV2RGB start point
    fread(buff, 1, IMG_W * IMG_H * 3 / 2, fp);
    nowFrame++;
    nFrame++;

    SDL_UpdateTexture(texture, NULL, buff, IMG_W);

    rect->x = 0;
    rect->y = 0;
    rect->w = WINDOW_W;
    rect->h = WINDOW_H;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_RenderPresent(renderer);
    
    printf("%d frame\n", nowFrame);
    endTime = SDL_GetTicks(); // YUV2RGB end point
    FTime = endTime - startTime; // Time taken per frame
    sumFTime += FTime;
    printf( "프레임 당 소요시간 : %dms\n", FTime);
}

// Print result for a play
void printResult()
{
    printf( "재생 완료. \n" );
    printf( "프레임 당 평균 소요시간: %fms\n", (float)sumFTime / nFrame );
    printf( "총 소요시간: %dms\n", sumFTime * nFrame );
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
    // SDL init
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
            bool quit = false; // Main loop flag
            
            SDL_Event event;
            SDL_Rect rect;
            
            memset(buff, 0, IMG_W * IMG_H * 3 / 2);
            
            // Main loop
            while( !quit )
            {
                // Event handling
                while( SDL_PollEvent( &event ) != 0 )
                {
                    // Event - quit
                    if( event.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    // Event - keydown
                    else if( event.type == SDL_KEYDOWN){
                        // Play & Pause
                        if( event.key.keysym.sym == SDLK_SPACE ){
                            play = !play;
                            printResult();
                            // Play
                            if( play == true ){
                                printf("Play\n");
                                // Play from the first frame at the end of the file
                                if( nowFrame == IMG_F ){
                                    printf(" Rewind \n");
                                    rewind( fp );
                                    nowFrame = 0;
                                }
                                nFrame = 0;
                                sumFTime = 0;
                            // Pause
                            }
                            else if( play == false ){
                                printf("Pause\n");
                            }
                        }
                        // Move to previous frame
                        else if( event.key.keysym.sym == SDLK_LEFT ){
                            if(nowFrame <= 1)
                            {
                                printf("First frame.\n");
                            }
                            else
                            {
                                sumFTime = 0;
                                printf("Previous Frame\n");
                                nowFrame = nowFrame - 2;
                                nFrame = 1;
                                fseek(fp, -(IMG_W * IMG_H * 3 / 2 * 2) ,SEEK_CUR);
                                displayFrame(&rect);
                                printResult();
                            }
                            
                        }
                        // Move to next frame
                        else if( event.key.keysym.sym == SDLK_RIGHT ){
                            if( nowFrame >= 30)
                            {
                                printf("Last Frame.\n");
                            }
                            else{
                                sumFTime = 0;
                                printf("Next Frame\n");
                                nFrame = 1;
                                fseek(fp, 1 ,SEEK_CUR);
                                displayFrame(&rect);
                                printResult();
                            }
                        }
                    }
                }
                    // Display until the end of the file
                if( play == true && nowFrame < IMG_F ){
                    displayFrame(&rect);
                    SDL_Delay(10);
                    
                    // Stop playing at the end of the file
                    if(nowFrame == IMG_F){
                        play = !play;
                        printResult();
                        sumFTime = 0;
                    }
                }
                
            }
        }
    }
    SDLclose();
    return 0;
}
