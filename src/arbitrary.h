#ifndef ARBITRARY_H
#define ARBITRARY_H
#include <raylib.h>
#include <stdint.h>

// RMB menu information, like mouse position at the time of click, and where the menu should be drawn
//
typedef struct {
	Vector2 mpos;
	Rectangle spos;
	Vector2 tl_rmb_menu_pos;
	int64_t status; // 0 if good, negative for errors, TODO: document errors
} RmbMenu;

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
	Color color;		// 4 bytes
} Cell;					// 232 bits


void swapTile(Cell* first, Cell* second);
void DrawRectangleWithBorder(int x, int y, int width, int height, Color fillColor, Color borderColor);
void draw_rmb_menu_tile(RmbMenu *rmb_menu, bool *show_rmb_menu_tile);
void Draw_selector_tooltip(int x, int y, int width, int height, uint32_t material, Font *font);
const char* tf_str(bool test);
const char* str_mat(uint32_t material);
int32_t GetMouse_X_safe();
int32_t GetMouse_Y_safe();

#endif
