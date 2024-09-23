//--------------------------
// - Atlas 3D -
// Renderer 3D Vertex Shader
// --------------------------

#version 450 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec3  a_Normal;
layout(location = 2) in vec2  a_TexCoord;

layout(location = 3) in vec3  a_AmbientColor;
layout(location = 4) in vec3  a_DiffuseColor;
layout(location = 5) in vec3  a_SpecularColor;
layout(location = 6) in float a_Shininess;

layout(location = 7) in int   a_DiffuseTextureIndex;
layout(location = 8) in int   a_SpecularTextureIndex;

layout(location = 9) in int   a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_CameraPosition;
};

struct VertexData
{
	vec3  Position;
	vec3  Normal;
	vec2  TexCoord;

	vec3  AmbientColor;
	vec3  DiffuseColor;
	vec3  SpecularColor;
	float Shininess;
};

layout (location = 0) out VertexData VertexOutput;
layout (location = 7) out flat int   v_DiffuseTextureIndex;
layout (location = 8) out flat int   v_SpecularTextureIndex;
layout (location = 9) out flat int   v_EntityID;

void main()
{
	VertexOutput.Position      = a_Position;
	VertexOutput.Normal        = a_Normal;
	VertexOutput.TexCoord      = a_TexCoord;

	VertexOutput.AmbientColor  = a_AmbientColor;
	VertexOutput.DiffuseColor  = a_DiffuseColor;
	VertexOutput.SpecularColor = a_SpecularColor;
	VertexOutput.Shininess     = a_Shininess;

	v_DiffuseTextureIndex      = a_DiffuseTextureIndex;
	v_SpecularTextureIndex     = a_SpecularTextureIndex;

	v_EntityID                 = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}