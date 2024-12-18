//--------------------------
// - Atlas Screen-Space Renderer -
// Greyscale Fragment Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

layout (std140, binding = 3) uniform Settings
{
	float u_Strength;
	float u_KernelOffset;
};

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 0) out vec4 o_Color;

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
    vec4 color = texture(u_screenTexture, v_TexCoords);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;

	o_Color = vec4(average, average, average, 1.0);
}