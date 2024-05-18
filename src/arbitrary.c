#include "arbitrary.h"
#include <stdint.h>

static const char *MaterialTypeStrings[] = {
    "Empty",
    "Sand",
    "Water",
    "Stone",
    "Steam",
    "Spawner",
    "Void"
};

void swapTile(Cell* first, Cell* second) {
	Cell temp = *second;
	*second = *first;
	*first = temp;
	return;
}

const char* tf_str(bool test) {
	if (test) {
		return "true";
	}
	return "false";
}


const char* str_mat(uint32_t material) {
	return MaterialTypeStrings[material];
}

// Draws a rectangle {x,y,w,h,} and makes the outmost pixels borderColor
void DrawRectangleWithBorder(int x, int y, int width, int height, Color fillColor, Color borderColor) {
	DrawRectangle(x, y, width, height, fillColor);
	DrawRectangleLinesEx((Rectangle){ x, y, width, height }, 1, borderColor);
}

// This function draws the actual menu when RMB is pressed
void draw_rmb_menu_tile(RmbMenu *rmb_menu, bool *show_rmb_menu_tile) {
	if (rmb_menu->status != 0) {
		*show_rmb_menu_tile = false;
		return;
	}
	int32_t mx = rmb_menu->mpos.x;
	int32_t my = rmb_menu->mpos.y;
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(rmb_menu->mpos, rmb_menu->spos) == false && *show_rmb_menu_tile == false)  {
		rmb_menu->spos.x = mx;
		rmb_menu->spos.y = my;
		rmb_menu->spos.width = 180;
		rmb_menu->spos.height = 240;
		*show_rmb_menu_tile = true;
	} if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && *show_rmb_menu_tile && !CheckCollisionPointRec(rmb_menu->mpos, rmb_menu->spos))
    {
        *show_rmb_menu_tile = !*show_rmb_menu_tile;
        rmb_menu->spos = (Rectangle){ 0 };
    }
    
	if (*show_rmb_menu_tile) {
		DrawRectangleWithBorder(rmb_menu->spos.x, rmb_menu->spos.y, 180,240, GRAY, BLUE);
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)  && CheckCollisionPointRec(rmb_menu->mpos, rmb_menu->spos) == false ) {
		*show_rmb_menu_tile = false;
		rmb_menu->status = -1;
	}
	return;
}

int32_t GetMouse_X_safe() {
	int ret = GetMouseX();
	int sw = GetScreenWidth();
	if (ret > sw)
	{
		return sw;
	}
	else if (ret < 0)
	{
		return 0;
	}
	return ret;
}

int32_t GetMouse_Y_safe() {
	int ret = GetMouseY();
	int sh = GetScreenHeight();
	if (ret > sh)
	{
		return sh;
	}
	else if (ret < 0)
	{
		return 0;
	}
	return ret;
}

// This function draws the tooltip (material name) when hovering over a material in the top left selector
void Draw_selector_tooltip(int x, int y, int width, int height, uint32_t material, Font *font) {
	Vector2 mpos = { GetMouse_X_safe(), GetMouse_Y_safe()};
	const char* str = str_mat(material);
	if (CheckCollisionPointRec(mpos, (Rectangle){ x, y, width, height })) {
		DrawRectangleWithBorder(mpos.x,mpos.y,120,24,DARKGRAY,BLACK);
		DrawTextEx(*font, str, (Vector2){mpos.x+120-(MeasureTextEx(*font,str,20,0)).x,mpos.y+2},20,0,WHITE);
        // DrawTextEx(*font, TextFormat("%s", (const char *)str_mat(material)), (Vector2){400,400}, 20, 0, WHITE);
	}
}
