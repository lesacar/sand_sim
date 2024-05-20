#include "render.h"

void UpdateScreenImage(Cell (*grid)[ROWS], Image* screenImage) {
    // Calculate the scaled dimensions of the grid

    // Resize the screen image to match the scaled grid dimensions
    // *screenImage = GenImageColor(scaledWidth, scaledHeight, BLACK);

    // Cast data pointer to Color*
    Color* imageData = (Color*)screenImage->data;

    // Load grid data into the screen image, scaling by BLOCK_SIZE
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            Color color = grid[i][j].color; // Assuming grid contains color information
            // Set color for the entire block
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    // Calculate the coordinates in the scaled image
                    int scaledX = i * BLOCK_SIZE + x;
                    int scaledY = j * BLOCK_SIZE + y;
                    // Set pixel color in the scaled image
                    imageData[scaledY * SCREEN_WIDTH + scaledX] = color;
                }
            }
        }
    }
}
