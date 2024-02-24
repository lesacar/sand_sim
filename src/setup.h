#ifndef SETUP_H
#define SETUP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include <immintrin.h>
#include <raylib.h>

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720
#define BLOCK_SIZE 2
#define ROWS (SCREEN_HEIGHT / BLOCK_SIZE)
#define COLS (SCREEN_WIDTH / BLOCK_SIZE)
#define NUM_THREADS 4

typedef enum
{
	Empty,
	Sand,
	Water,
	Stone
} MaterialTypes;

typedef struct
{
	int startRow;
	int endRow;
	int column;
	Color color;
} Batch;

typedef struct
{
	uint8_t material;	// 8 bits
	float friction;		// 32 bits
	bool isFreeFalling; // 8 bits ?
	float velocityX;	// 32 bits
	float velocityY;	// 32 bits
	int32_t mass;		// 32 bits
	float spreadFactor; // 32 bits
} Cell;					// 176 bits

// Function prototype
int32_t setup_stuff(int32_t sc_wi, int32_t sc_he, const char *WindowTitle, int32_t log_lvl, bool fullscreen);
int32_t set_monitor_and_fps(int32_t monitor);
void sand(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]);
void updateWater(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]);

void spawnSandBrush(Cell (*grid)[ROWS], int32_t mouseX, int32_t mouseY, int32_t brushSize, int32_t material, bool brushMode);
#endif /* SETUP_H */
