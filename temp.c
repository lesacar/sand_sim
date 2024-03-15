#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
			DrawText(TextFormat("%d",GetFPS()), 20, 20, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
