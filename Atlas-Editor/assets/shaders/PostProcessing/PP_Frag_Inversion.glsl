//--------------------------
// - Atlas Post-Processing -
// Inversion Fragment Shader
// --------------------------

#version 450 core

layout (location = 2) out vec4 o_Color;

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

void main()
{
	o_Color = vec4(vec3(1.0 - texture(u_screenTexture, v_TexCoords)), 1.0);
}