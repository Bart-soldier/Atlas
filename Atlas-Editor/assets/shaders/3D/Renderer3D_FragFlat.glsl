//--------------------------
// - Atlas 3D -
// Renderer 3D Flat Color Fragment Shader
// --------------------------

#version 450 core

struct VertexData
{
	vec3  Position;
	vec3  Normal;
	vec2  TexCoord;
	mat3  TBN;

	vec3  AmbientColor;
	vec3  DiffuseColor;
	vec3  SpecularColor;
	float Shininess;
};

layout (location = 0) in flat int   v_EntityID;
layout (location = 1) in VertexData VertexInput;

layout (std140, binding = 0) uniform Settings
{
	float u_Gamma;
};

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;

void main()
{
	o_Color            = vec4(pow(VertexInput.DiffuseColor.rgb, vec3(u_Gamma)), 1.0);
	o_EntityID         = v_EntityID;
}