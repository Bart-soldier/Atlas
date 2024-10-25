//--------------------------
// - Atlas Post-Processing -
// Inversion Fragment Shader
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
	o_PostProcessColor = vec4(vec3(1.0 - texture(u_screenTexture, v_TexCoords)), 1.0);
}