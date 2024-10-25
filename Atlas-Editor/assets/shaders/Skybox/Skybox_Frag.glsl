//--------------------------
// - Atlas Skybox -
// Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec3 v_TexCoords;

layout (binding = 0) uniform samplerCube u_Skybox;

layout (std140, binding = 0) uniform Settings
{
	float u_Gamma;
};

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;

void main()
{
	vec4 skybox        = texture(u_Skybox, v_TexCoords);
	o_Color            = vec4(pow(skybox.rgb, vec3(u_Gamma)), skybox.a);
	o_EntityID         = -1;
}