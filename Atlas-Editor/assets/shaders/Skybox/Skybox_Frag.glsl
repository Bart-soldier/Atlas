//--------------------------
// - Atlas Skybox -
// Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec3 v_TexCoords;

layout (binding = 0) uniform samplerCube u_Skybox;

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;
layout (location = 2) out vec4 o_PostProcessColor;

void main()
{
	o_Color            = texture(u_Skybox, v_TexCoords);
	o_PostProcessColor = o_Color;
}