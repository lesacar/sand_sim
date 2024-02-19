#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "arg_handler.h"
#include "setup.h"


#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720

int main(int argc, char** argv)
{
    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "RAYtitle", LOG_ERROR);
    // get desired monitor to spawn on (default 1st monitor) and create a window on that monitor and set the fps to it's refresh rate
    int current_monitor = handle_arguments(argc, argv); if (current_monitor < 0) { exit(EXIT_FAILURE); } 
    set_monitor_and_fps(current_monitor); 

    float cur_dt = 0.0f; float last_dt = cur_dt;
    
    while (!WindowShouldClose()) {
        last_dt = cur_dt; cur_dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawText(TextFormat("%.2f", (double)((float)1.0/(float)cur_dt)), 20, 20, 20, WHITE);
        EndDrawing(); 
    }
    return 0;
}
