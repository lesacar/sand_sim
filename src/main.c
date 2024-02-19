#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include "arg_handler.h"
#include "setup.h"


#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720

#define ROWS 180
#define COLS 360
const int32_t cellWidth = SCREEN_WIDTH / COLS;
const int32_t cellHeight = SCREEN_HEIGHT / ROWS;

typedef struct Cell
{
	int16_t i;
	int16_t j;
	uint8_t material;
	bool doneFalling;
} Cell;

int32_t main(int32_t argc, char** argv)
{
	Cell grid[COLS][ROWS];
	srand(time(0));
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            grid[i][j].i = i;
            grid[i][j].j = j;
        }
    }


    setup_stuff(SCREEN_WIDTH, SCREEN_HEIGHT, "RAYtitle", LOG_ERROR);
    // get desired monitor to spawn on (default 1st monitor) and create a window on that monitor and set the fps to it's refresh rate
    int32_t current_monitor = handle_arguments(argc, argv); if (current_monitor < 0) { exit(EXIT_FAILURE); } 
    set_monitor_and_fps(current_monitor); 

    float cur_dt = 0.0f; float last_dt = cur_dt;
    while (!WindowShouldClose()) {
        last_dt = cur_dt; cur_dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(BLACK);
		
		for (size_t i = 0; i < COLS; i++)
		{
			for (size_t j = 0; j < ROWS; j++)
			{
				
			}
			
		}
		DrawText(TextFormat("%.2f", (double)((float)1.0/(float)cur_dt)), 20, 20, 20, RED);
        
        EndDrawing();
    }
    return 0;
}
