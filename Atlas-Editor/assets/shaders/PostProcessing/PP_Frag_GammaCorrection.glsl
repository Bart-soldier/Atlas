//--------------------------
// - Atlas Post-Processing -
// Gamma Correction Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

layout (binding = 0) uniform sampler2D u_screenTexture;

layout (std140, binding = 3) uniform Settings
{
	float u_Strength;
	float u_KernelOffset;
};

layout (location = 0) out vec4 o_Color;

void main()
{
    vec4 color = texture(u_screenTexture, v_TexCoords);

	o_Color = vec4(pow(color.rgb, vec3(1.0/u_Strength)), color.a);

	// TODO: Remove and keep above code when multiple PP is fixed
//	vec3 hdrColor = texture(u_screenTexture, v_TexCoords).rgb;
//	vec3 toneMap = vec3(1.0) - exp(-hdrColor * 1.0);
//
//	toneMap = pow(toneMap, vec3(1.0/u_Strength));
//	o_Color = vec4(toneMap, 1.0);
}