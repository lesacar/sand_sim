#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

uniform int grid[1440][720]; // Assuming maximum dimensions

void main()
{
    int block = grid[int(fragCoord.x)][int(fragCoord.y)];
    if (block > 0)
        fragColor = vec4(201.0 / 255.0, 170.0 / 255.0, 127.0 / 255.0, 1.0); // Sand block color
    else
        discard; // Don't render empty cells
}
