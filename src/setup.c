#include "setup.h"
#include <raylib.h>

int32_t setup_stuff(int32_t sc_wi, int32_t sc_he, const char* WindowTitle, int32_t log_lvl, bool fullscreen){
    SetTraceLogLevel(log_lvl);
	if (fullscreen)
	{
		SetConfigFlags(FLAG_FULLSCREEN_MODE);
	}

    InitWindow(sc_wi,sc_he, WindowTitle);
    return 0;
}

int32_t set_monitor_and_fps(int32_t monitor){
    SetWindowMonitor(monitor-1);
    const int32_t initial_fps = GetMonitorRefreshRate(monitor -1);
    SetTargetFPS(initial_fps);
    return 0;
}



// Function to spawn sand brush
void spawnSandBrush(Cell (*grid)[ROWS], int32_t mouseX, int32_t mouseY, int32_t brushSize, int32_t material)
{
    int startX = mouseX / BLOCK_SIZE - brushSize / 2;
    int startY = mouseY / BLOCK_SIZE - brushSize / 2;

    startX = (startX < 0) ? 0 : startX;
    startY = (startY < 0) ? 0 : startY;
    startX = (startX > COLS - brushSize) ? COLS - brushSize : startX;
    startY = (startY > ROWS - brushSize) ? ROWS - brushSize : startY;

    int centerX = startX + brushSize / 2;
    int centerY = startY + brushSize / 2;

    for (int i = startX; i < startX + brushSize; i++)
    {
        for (int j = startY; j < startY + brushSize; j++)
        {
            float distance = sqrt(pow(i - centerX, 2) + pow(j - centerY, 2));

            if (brushSize == 1)
            {
                grid[i][j].material = 1;
            }
            else if ((float)rand() / RAND_MAX > 0.8f && distance <= brushSize / 2)
            {
                grid[i][j].material = 1;
            }
        }
    }
}
