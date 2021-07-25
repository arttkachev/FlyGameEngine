#version 330 core

layout (location = 0) in vec3 pos; // for location of vertices (location = 0) 0 means location data
layout (location = 1) in vec2 texCoordsData; // for location of texture UV (so vec2) (location = 1) because of interleaved memory pattern we use 1 means UV data

// matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords; // will be passing out our tex coords to frag shader

void main()
{
    // get gl_Position in Normalized Device Coords. A right sequence of multiplication read from right to left
    gl_Position = projection * view * model * vec4(pos, 1.0); // pos comes in in local space. Read from right to left, it's correct sequence to multiply
    TexCoords = texCoordsData; // vertex shader just passes on tex coords
};