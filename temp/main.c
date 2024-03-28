#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	SetTargetFPS(0);
	InitWindow(1280, 720, "Window");
	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);
		DrawText(TextFormat("%d",GetFPS()), 10, 10, 20, WHITE);
		DrawText(TextFormat("%d",GetMouseX()), 10, 40, 20, WHITE);
		EndDrawing();
	}
}
