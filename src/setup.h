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


// Underline on
#define UON = "\033[4m";
// Underline off
#define UOFF = "\033[0m";

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720
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
} ConfigData;

typedef struct {
	bool show;
	Rectangle position;
	const char *title;
	const char *text;
	uint8_t buttons;
	bool dragging;
	Vector2 drag_offset;
} MsgBox;

typedef struct {
	Vector2 mpos;
	Rectangle spos;
	Vector2 tl_rmb_menu_pos;
	int64_t status; // 0 if good, negative for errors, TODO: document errors
} RmbMenu;

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
	MatCount // !! THIS VALUE SHOULD ALWAYS BE THE LAST ENUM ELEMENT 
} MaterialTypes;

extern const char *MaterialTypeStrings[];

typedef struct
{
	uint32_t material;	// 32 bits
	/// Working material (child material)
	uint32_t w_material; // 32 bits
	float friction;		// 32 bits
	bool isFreeFalling; // 8 bits ?
	float velocityX;	// 32 bits
	float velocityY;	// 32 bits
	int32_t mass;		// 32 bits
	float spreadFactor; // 32 bits
	Color color;		// 32 bits
} Cell;					// 232 bits

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
const char* tf_str(bool test);
const char* str_mat(uint32_t material);
int32_t GetMouse_X_safe();
int32_t GetMouse_Y_safe();

#endif /* SETUP_H */
