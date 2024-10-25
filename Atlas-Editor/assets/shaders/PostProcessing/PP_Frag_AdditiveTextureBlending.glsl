//--------------------------
// - Atlas Post-Processing -
// Additive Texture Blending Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D[2] u_screenTexture;

layout (location = 0) out vec4 o_Color;

void main()
{
    vec3 firstTexColor  = texture(u_screenTexture[0], v_TexCoords).rgb;
    vec3 secondTexColor = texture(u_screenTexture[1], v_TexCoords).rgb;

    firstTexColor += secondTexColor;

    o_Color = vec4(firstTexColor, 1.0);
}