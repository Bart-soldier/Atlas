//--------------------------
// - Atlas 3D -
// Renderer 3D PBR GBuffer Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- STRUCTS ---------- */
/* ------------------------------ */

//struct VertexData
//{
//	vec3  Position;
//	vec3  Normal;
//	vec2  TexCoord;
//	mat3  TBN;
//
//	vec3  Color;
//	float Metallic;
//	float Roughness;
//};

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0)   in flat int   v_EntityID;
layout (location = 1)   in      vec3  v_Position;
layout (location = 2)   in      vec3  v_Normal;
layout (location = 3)   in      vec2  v_TexCoord;
layout (location = 4)   in      vec3  v_Color;
layout (location = 5)   in      float v_Metallic;
layout (location = 6)   in      float v_Roughness;
layout (location = 7)   in      mat3  v_TBN;
layout (location = 10)  in flat vec3  v_Albedo_Normal_Metallic_TexIndex;    // X: Albedo,    Y: Normal, Z: Metallic
layout (location = 11)  in flat vec3  v_Roughness_AO_Displacement_TexIndex; // X: Roughness, Y: AO,     Z: Displacement

layout (binding = 0) uniform sampler2D u_Textures[32];

layout (std140, binding = 0) uniform Settings
{
	float u_Gamma;
	float u_ParallaxScale;
	float u_BloomThreshold;
};

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec4 u_CameraPosition;
};

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 1) out int  o_EntityID;
layout (location = 2) out vec4 o_Position;
layout (location = 3) out vec4 o_Normal;
layout (location = 4) out vec4 o_Albedo;
layout (location = 5) out vec4 o_Metallic_Roughness_AO; // R: Metallic, G: Roughness, B: AO

/* ------------------------------ */
/* ----- METHOD DEFINITIONS ----- */
/* ------------------------------ */

vec2  GetFinalTexCoords();
vec4  GetAlbedoOutput(vec2 texCoord);
vec3  GetNormalOutput(vec2 texCoord);
float GetMetallicOutput(vec2 texCoord);
float GetRoughnessOutput(vec2 texCoord);
float GetAOOutput(vec2 texCoord);

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	vec2 texCoord            = GetFinalTexCoords();

	vec4 albedo              = GetAlbedoOutput(texCoord);
	vec3 normal              = GetNormalOutput(texCoord);
	vec3 metallicRoughnessAO = vec3(GetMetallicOutput(texCoord), GetRoughnessOutput(texCoord), GetAOOutput(texCoord));

	o_EntityID               = v_EntityID;
	o_Position               = vec4(v_Position, 1.0);
	o_Normal                 = vec4(normal, 1.0);
	o_Albedo                 = albedo;
	o_Metallic_Roughness_AO  = vec4(metallicRoughnessAO, 1.0);
}

/* ------------------------------ */
/* --- METHOD IMPLEMENTATIONS --- */
/* ------------------------------ */

vec2 ParallaxMapping(uint displacementTexIndex, vec2 texCoord, vec3 viewDirection)
{
    const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDirection), 0.0));

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

	vec2 P = viewDirection.xy * u_ParallaxScale; 
    vec2 deltaTexCoord = P / numLayers;

	vec2  currentTexCoord      = texCoord;
	float currentDepthMapValue = texture(u_Textures[displacementTexIndex], currentTexCoord).r;
  
	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoord -= deltaTexCoord;
		currentDepthMapValue = texture(u_Textures[displacementTexIndex], currentTexCoord).r;
		currentLayerDepth += layerDepth;
	}

	vec2 prevTexCoord = currentTexCoord + deltaTexCoord;

	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(u_Textures[displacementTexIndex], prevTexCoord).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0 - weight);

	return finalTexCoord;
}

vec2 GetFinalTexCoords()
{
	vec2 texCoord = v_TexCoord;

	uint displacementTexIndex = uint(v_Roughness_AO_Displacement_TexIndex.z);

	if(displacementTexIndex != 0)
	{
		vec3 viewDirection = normalize(transpose(v_TBN) * (u_CameraPosition.xyz - v_Position));
		texCoord = ParallaxMapping(displacementTexIndex, v_TexCoord, viewDirection);
	}

	if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
	{
		discard;
	}

	return texCoord;
}

vec4 GetAlbedoOutput(vec2 texCoord)
{
	vec4 diffuseColor = texture(u_Textures[int(v_Albedo_Normal_Metallic_TexIndex.x)], texCoord);

	if (diffuseColor.a == 0.0)
	{
		discard;
	}

	diffuseColor = vec4(pow(diffuseColor.rgb, vec3(u_Gamma)) * pow(v_Color.rgb, vec3(u_Gamma)), diffuseColor.a);

	return diffuseColor;
}

vec3 GetNormalOutput(vec2 texCoord)
{
	vec3 vertexNormal = normalize(v_Normal);

	int normalTexIndex = int(v_Albedo_Normal_Metallic_TexIndex.y);

	if(normalTexIndex != 0)
	{
		vec3 normalMap = texture(u_Textures[normalTexIndex], texCoord).rgb;
		vertexNormal = normalMap * 2.0 - 1.0;
		vertexNormal = normalize(v_TBN * vertexNormal);
	}

	return vertexNormal;
}

float GetMetallicOutput(vec2 texCoord)
{
	float metallic = v_Metallic;

	int metallicTexIndex = int(v_Albedo_Normal_Metallic_TexIndex.z);

	if(metallicTexIndex != 0)
	{
		metallic = texture(u_Textures[metallicTexIndex], texCoord).r;
	}

	return metallic;
}

float GetRoughnessOutput(vec2 texCoord)
{
	float roughness = v_Roughness;

	int roughnessTexIndex = int(v_Roughness_AO_Displacement_TexIndex.x);

	if(roughnessTexIndex != 0)
	{
		roughness = texture(u_Textures[roughnessTexIndex], texCoord).r;
	}

	return roughness;
}

float GetAOOutput(vec2 texCoord)
{
	float ao = 1.0;

	int aoTexIndex = int(v_Roughness_AO_Displacement_TexIndex.y);

	if(aoTexIndex != 0)
	{
		ao = texture(u_Textures[aoTexIndex], texCoord).r;
	}

	return ao;
}