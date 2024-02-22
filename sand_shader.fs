#version 330

in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D screenTexture;
uniform vec2 screenSize;

void main()
{
    vec2 uv = fragCoord / screenSize;
    vec4 color = texture(screenTexture, uv); // Get color from screen texture

    // Apply some manipulation to the color here based on the sand simulation
    // For example, you might darken the color based on sand density or displacement

    fragColor = color;
}
