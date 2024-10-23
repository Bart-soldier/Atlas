//--------------------------
// - Atlas Post-Processing -
// Gamma Correction Fragment Shader
// --------------------------

#version 450 core

layout(location = 2) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoords;

layout(binding = 0) uniform sampler2D u_screenTexture;

layout (std140, binding = 3) uniform KernelOffset
{
	float u_Gamma;
};

void main()
{
    vec4 color = texture(u_screenTexture, v_TexCoords);

	o_Color = vec4(pow(color.rgb, vec3(1.0/u_Gamma)), color.a);
}