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

	vec3  AmbientColor;
	vec3  DiffuseColor;
	vec3  SpecularColor;
	float Shininess;
};

struct LightData
{
	vec4 Position;
	vec4 Color;
	vec4 Direction; // w is a flag to indicate if light direction is spot direction

	float Radius;
	float Intensity;
	vec2 CutOffs; // (inner, outer); negative value means cutoff is disabled

	float AmbientStrength;
	float DiffuseStrength;
	float SpecularStrength;
};

layout (location = 0) in VertexData VertexInput;
layout (location = 7) in flat uint  v_DiffuseTextureIndex;
layout (location = 8) in flat uint  v_SpecularTextureIndex;
layout (location = 9) in flat int   v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout (std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};

layout (std140, binding = 1) uniform LightCount
{
	uint u_LightCount;
};

layout (std430, binding = 2) buffer Lights
{
	LightData u_Lights[];
};

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;
layout (location = 2) out vec4 o_PostProcessColor;

void main()
{
	o_Color            = vec4(VertexInput.DiffuseColor, 1.0);
	o_EntityID         = v_EntityID;
	o_PostProcessColor = o_Color;
}