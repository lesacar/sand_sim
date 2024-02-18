#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include "arg_handler.h"
#include "setup.h"


#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720

int main(int argc, char** argv)
{
    char cwd[256+32];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
   } else {
       perror(strerror(errno));
       return 1;
   }
   strcat(cwd, "/dll/libraylib.so");
    void* raylib_handle = dlopen(cwd, RTLD_LAZY);
    if (!raylib_handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }
    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "RAYtitle", LOG_ERROR);
    // get desired monitor to spawn on (default 1st monitor) and create a window on that monitor and set the fps to it's refresh rate
    int current_monitor = handle_arguments(argc, argv); if (current_monitor < 0) { exit(EXIT_FAILURE); } 
    set_monitor_and_fps(current_monitor); 

    float cur_dt = 0.0f;
    float last_dt = cur_dt;
    
    while (!WindowShouldClose()) {
        last_dt = cur_dt;
        cur_dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(WHITE);
        
        DrawText(TextFormat("%.2f", (double)((float)1.0/(float)cur_dt)), 20, 20, 20, BLACK);
        EndDrawing(); 
    }
    return 0;
}
