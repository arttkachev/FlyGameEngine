#version 330 core

layout (location = 0) in vec3 pos; // for location of vertices (location = 0) 0 means location data
layout (location = 1) in vec2 texCoordsData; // for location of texture UV (so vec2) (location = 1) because of interleaved memory pattern we use 1 means UV data

out vec2 TexCoords; // will be passing out our tex coords to frag shader 
uniform vec2 posOffset;

void main()
{
    gl_Position = vec4(pos.x + posOffset.x, pos.y + posOffset.y, pos.z, 1.0);
    TexCoords = texCoordsData; // vertex shader just passes on tex coords
};