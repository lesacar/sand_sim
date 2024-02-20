// shader.fs
#version 460 core

// Define grid dimensions
#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 720
#define BLOCK_SIZE 4
#define ROWS (SCREEN_HEIGHT / BLOCK_SIZE)
#define COLS (SCREEN_WIDTH / BLOCK_SIZE)

// Input from vertex shader
in vec2 TexCoord;

// Output
out vec4 FragColor;

// Uniform texture sampler
uniform sampler2D gridTexture;

void main() {
    // Sample grid texture using texture coordinates
    vec4 gridColor = texture(gridTexture, TexCoord);
    
    // Output the sampled color
    FragColor = gridColor;
}
