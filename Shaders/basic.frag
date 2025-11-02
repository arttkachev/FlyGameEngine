#version 330 core

in vec2 TexCoords; // passing in and out our data between shaders
out vec4 frag_color; 

uniform sampler2D texSampler; // sampler2D is a built-in type in GLSL
uniform sampler2D texSampler2; // the second sampler to blend 2 textures
uniform vec4 vertColor; // unifrom is a kind of var we have an access from any shader (think of this like global vars)
void main()
{
    //frag_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //frag_color = vertColor;

    // texture() is a built-in method in GLSL to use a texture for fragments
    // args(sampler, texture coordinates)
    // it captures color from the texture and assigns it to frag_color
    // texture() returns pixel color for that particular pixel on the screen
    //frag_color = texture(texSampler, TexCoords);

    // blend two texture (mix() method in GLSL)
    // args: (texture 1 to blend, texture 2 to blend, how much to blend (from 0.0 to 1.0))
    frag_color = mix(texture(texSampler, TexCoords), texture(texSampler2, TexCoords), 0.2);
};