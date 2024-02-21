#ifndef SETUP_H
#define SETUP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720
#define BLOCK_SIZE 2
#define ROWS (SCREEN_HEIGHT / BLOCK_SIZE)
#define COLS (SCREEN_WIDTH / BLOCK_SIZE)

// Function prototype
int32_t setup_stuff(int32_t sc_wi, int32_t sc_he, const char* WindowTitle, int32_t log_lvl, bool fullscreen);
int32_t set_monitor_and_fps(int32_t monitor);


struct Cell {
    uint8_t material;
	float friction;
    bool isFreeFalling;
    float velocityX;
    float velocityY;
	int mass;
	float spreadFactor;
};

typedef struct Cell Cell;

void spawnSandBrush(Cell (*grid)[ROWS], int32_t mouseX, int32_t mouseY, int32_t brushSize, int32_t material, bool brushMode);
#endif /* SETUP_H */
