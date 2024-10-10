//--------------------------
// - Atlas Post-Processing -
// Greyscale Fragment Shader
// --------------------------

#version 450 core

layout(location = 2) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoords;

layout(binding = 0) uniform sampler2D u_screenTexture;

void main()
{
    vec4 color = texture(u_screenTexture, v_TexCoords);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;

	o_Color = vec4(average, average, average, 1.0);
}