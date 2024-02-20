// sand_blocks.glsl
#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D gridTexture; // Texture containing the grid data
uniform vec2 screenSize;       // Screen size

void main()
{
    vec4 block = texture(gridTexture, fragCoord / screenSize);
    
    // Render rectangles only if the current cell contains material
    if (block.r > 0.0)
    {
        fragColor = vec4(201.0 / 255.0, 170.0 / 255.0, 127.0 / 255.0, 1.0); // Sand block color
    }
    else
    {
        discard; // Discard fragments for empty cells
    }
}
