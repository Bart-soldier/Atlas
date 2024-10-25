//--------------------------
// - Atlas Post-Processing -
// Reinhard Tone Mapping Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

layout (std140, binding = 3) uniform Settings
{
	float u_Strength;
	float u_KernelOffset;
};

layout (location = 2) out vec4 o_Color;

void main()
{
	vec3 hdrColor = texture(u_screenTexture, v_TexCoords).rgb;
	vec3 toneMap = vec3(1.0) - exp(-hdrColor * u_Strength);

	o_Color = vec4(toneMap, 1.0);
}