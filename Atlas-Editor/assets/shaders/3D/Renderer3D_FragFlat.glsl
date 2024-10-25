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
layout (location = 11)  in flat uint  v_DiffuseTextureIndex;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout (std140, binding = 0) uniform Settings
{
	float u_Gamma;
};

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;

void main()
{
	vec4 diffuseColor;

	if(v_DiffuseTextureIndex == 0)
	{
		diffuseColor = vec4(VertexInput.DiffuseColor, 1.0);
	}
	else
	{
		diffuseColor  = texture(u_Textures[v_DiffuseTextureIndex], VertexInput.TexCoord);
	}

	// Alpha discard on diffuse
	if (diffuseColor.a == 0.0)
	{
		discard;
	}

	o_Color            = vec4(pow(diffuseColor.rgb, vec3(u_Gamma)), 1.0);
	o_EntityID         = v_EntityID;
}