//--------------------------
// - Atlas 3D -
// Renderer 3D Vertex Shader
// --------------------------

#version 450 core

layout(location = 0)  in int   a_EntityID;

layout(location = 1)  in vec3  a_Position;
layout(location = 2)  in vec3  a_Normal;
layout(location = 3)  in vec2  a_TexCoord;
layout(location = 4)  in vec3  a_Tangent;

layout(location = 5)  in vec3  a_AmbientColor;
layout(location = 6)  in vec3  a_DiffuseColor;
layout(location = 7)  in vec3  a_SpecularColor;
layout(location = 8)  in float a_Shininess;

layout(location = 9)  in uint  a_DiffuseTextureIndex;
layout(location = 10) in uint  a_SpecularTextureIndex;
layout(location = 11) in uint  a_NormalMapTextureIndex;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec4 u_CameraPosition;
};

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

layout (location = 0)   out flat int   v_EntityID;
layout (location = 1)   out VertexData VertexOutput;
layout (location = 11)  out flat uint  v_DiffuseTextureIndex;
layout (location = 12)  out flat uint  v_SpecularTextureIndex;
layout (location = 13)  out flat uint  v_NormalMapTextureIndex;

void main()
{
	v_EntityID                  = a_EntityID;

	VertexOutput.Position       = a_Position;
	VertexOutput.TexCoord       = a_TexCoord;
	VertexOutput.Normal         = a_Normal;

	VertexOutput.AmbientColor   = a_AmbientColor;
	VertexOutput.DiffuseColor   = a_DiffuseColor;
	VertexOutput.SpecularColor  = a_SpecularColor;
	VertexOutput.Shininess      = a_Shininess;

	v_DiffuseTextureIndex       = a_DiffuseTextureIndex;
	v_SpecularTextureIndex      = a_SpecularTextureIndex;
	v_NormalMapTextureIndex     = a_NormalMapTextureIndex;

	if(a_NormalMapTextureIndex != 0)
	{
		vec3 T = normalize(a_Tangent);
		vec3 N = normalize(a_Normal);
		// Re-orthogonalize T with respect to N (Gram-Schmidt)
		T = normalize(T - dot(T, N) * N);
		// Retrieve perpendicular vector B with the cross product of T and N
		vec3 B = cross(N, T);

		VertexOutput.TBN = mat3(T, B, N);
	}

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}