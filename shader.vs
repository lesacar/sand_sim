// shader.vs
#version 460 core

// Input
layout(location = 0) in vec3 aPos;

// Output 
out vec2 TexCoord;

void main() {
    // Pass through the vertex position
    gl_Position = vec4(aPos, 1.0);
    
    // Generate texture coordinates based on vertex position
    TexCoord = (aPos.xy + vec2(1.0)) / 2.0;
}
