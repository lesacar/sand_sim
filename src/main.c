#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>



void CustomLog(int msgType, const char *text, va_list args)
{ 
  return;
}

int main(int argc, char** argv)
{
    int current_monitor = 1;
    SetTraceLogCallback(CustomLog); // call this before InitWindow() 
    InitWindow(720, 480, "RAYtitle");
    if (argc > 2) {
        if (strcmp(argv[1], "-m") == 0) { // Check if first argument is "-m"
            char *endptr;
            long num = strtol(argv[2], &endptr, 10); // Convert second argument to long

            if (errno == ERANGE || num < 1 || num > GetMonitorCount() || *endptr != '\0') {
                // Error handling for conversion failure or out-of-range value
                fprintf(stderr, "%s -m <N> (desired monitor number)\ndefault <N> is 1; Sets the desired monitor to spawn on\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            current_monitor = (int)num;
        }
    }

    SetWindowMonitor(current_monitor-1);

    
    const int initial_fps = GetMonitorRefreshRate(current_monitor -1);
    SetTargetFPS(initial_fps);
    float cur_dt = 0;
    float last_dt = cur_dt;
    
    while (!WindowShouldClose()) {
        last_dt = cur_dt;
        cur_dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(WHITE);
        
        DrawText(TextFormat("%.2f", 1.0/cur_dt), 20, 20, 20, BLACK);
        EndDrawing(); 
    }
    return 0;
}