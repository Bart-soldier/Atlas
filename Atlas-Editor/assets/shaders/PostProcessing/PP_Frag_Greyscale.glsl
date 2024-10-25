//--------------------------
// - Atlas Post-Processing -
// Greyscale Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

layout (std140, binding = 3) uniform Settings
{
	float u_Strength;
	float u_KernelOffset;
};

layout (location = 2) out vec4 o_PostProcessColor;

void main()
{
    vec4 color = texture(u_screenTexture, v_TexCoords);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;

	o_PostProcessColor = vec4(average, average, average, 1.0);
}