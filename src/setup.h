#ifndef SETUP_H
#define SETUP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include "arbitrary.h"
#include <math.h>
#include "arbitrary.h"
#include <immintrin.h>
#include <raylib.h>


// Underline on
#define UON = "\033[4m";
// Underline off
#define UOFF = "\033[0m";

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 640
#define BLOCK_SIZE 4
#define ROWS (SCREEN_HEIGHT / BLOCK_SIZE)
#define COLS (SCREEN_WIDTH / BLOCK_SIZE)
#define NUM_THREADS 4
#define MAX_CFG_SIZE 64*1024
#define MAX_CFG_LINE 100

#define COLOR_SAND \
	(Color) { 201, 170, 127, 255 }
#define COLOR_WATER \
	(Color) { 0, 0, 255, 255 }
#define COLOR_STONE \
	(Color) { 51, 83, 69, 255 }
#define COLOR_STEAM \
	(Color) { 42, 71, 94, 255 }
#define NOCOLOR \
	(Color) { 0, 0, 0, 0 }

typedef struct {
    bool is_cfg_read;
    int64_t cfg_file_size;
    char *cfg_buffer;
    int fps;
    bool brush_mode;
    int brush_size;
	bool read_map;
	bool wants_shader;
	int64_t random_grid_randomness;
} ConfigData;

typedef struct {
	bool show;
	Rectangle position;
	const char *title;
	char *text;
	uint8_t buttons;
	bool dragging;
	Vector2 drag_offset;
} MsgBox;

// Draw message box
uint8_t Draw_message_box(MsgBox *msgbox, Font *font);

// Function to parse configuration file into ConfigData struct
ConfigData parse_config_file(const char *cfg_file);

// Function to reload configuration file and update ConfigData struct
char *reload_config_file(const char *cfg_file, ConfigData *config);
void parse_config_variables(const char *cfg_buffer, ConfigData *config);

typedef enum
{
	Empty,
	Sand,
	Water,
	Stone,
	Steam,
	Spawner,
	VoidTile,
	Obsidian,
	MatCount // !! THIS VALUE SHOULD ALWAYS BE THE LAST ENUM ELEMENT 
} MaterialTypes;



// Function prototype
int32_t setup_stuff(int32_t sc_wi, int32_t sc_he, const char *WindowTitle, int32_t log_lvl, bool fullscreen);
int32_t set_monitor_and_fps(int32_t monitor);
void sand(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]);
void updateWater(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]);
void updateSteam(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]);
void updateSpawner(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]);
void updateVoidTile(Cell (*grid)[ROWS], Cell (*grid_duplicate)[ROWS]);
void spawnSandBrush(Cell (*grid)[ROWS], int32_t mouseX, int32_t mouseY, int32_t brushSize, uint32_t material, uint32_t w_material, bool brushMode);
Color rand_color_mat(uint32_t material);
#endif /* SETUP_H */
