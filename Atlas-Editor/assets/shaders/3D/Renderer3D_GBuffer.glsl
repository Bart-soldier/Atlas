//--------------------------
// - Atlas 3D -
// Renderer 3D GBuffer Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- STRUCTS ---------- */
/* ------------------------------ */

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

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0)   in flat int   v_EntityID;
layout (location = 1)   in VertexData VertexInput;
layout (location = 11)  in flat uint  v_DiffuseTextureIndex;
layout (location = 12)  in flat uint  v_SpecularTextureIndex;
layout (location = 13)  in flat uint  v_NormalMapTextureIndex;
layout (location = 14)  in flat uint  v_HeightMapTextureIndex;

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

layout (location = 0) out vec4 o_Position;
layout (location = 1) out vec4 o_NormalShininess; // rgb for normal and a for shininess
layout (location = 2) out vec4 o_AlbedoSpecular; // rgb for albedo and a for specular
layout (location = 3) out int  o_EntityID;

/* ------------------------------ */
/* ----- METHOD DEFINITIONS ----- */
/* ------------------------------ */

vec2 GetFinalTexCoords();
vec4 GetAlbedoSpecularOutput(vec2 texCoord);
vec4 GetNormalShininessOutput(vec2 texCoord);

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	vec2 texCoord = GetFinalTexCoords();

	vec4 albedoSpecular  = GetAlbedoSpecularOutput(texCoord);
	vec4 normalShininess = GetNormalShininessOutput(texCoord);

	o_Position        = vec4(VertexInput.Position, 1.0);
	o_NormalShininess = normalShininess;
	o_AlbedoSpecular  = albedoSpecular;
	o_EntityID        = v_EntityID;
}

/* ------------------------------ */
/* --- METHOD IMPLEMENTATIONS --- */
/* ------------------------------ */

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDirection)
{
    const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDirection), 0.0));

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

	vec2 P = viewDirection.xy * u_ParallaxScale; 
    vec2 deltaTexCoord = P / numLayers;

	vec2  currentTexCoord      = texCoord;
	float currentDepthMapValue = texture(u_Textures[v_HeightMapTextureIndex], currentTexCoord).r;
  
	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoord -= deltaTexCoord;
		currentDepthMapValue = texture(u_Textures[v_HeightMapTextureIndex], currentTexCoord).r;
		currentLayerDepth += layerDepth;
	}

	vec2 prevTexCoord = currentTexCoord + deltaTexCoord;

	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(u_Textures[v_HeightMapTextureIndex], prevTexCoord).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0 - weight);

	return finalTexCoord;
}

vec2 GetFinalTexCoords()
{
	vec2 texCoord;

	if(v_HeightMapTextureIndex == 0)
	{
		texCoord = VertexInput.TexCoord;
	}
	else
	{
		vec3 viewDirection = normalize(transpose(VertexInput.TBN) * (u_CameraPosition.xyz - VertexInput.Position));
		texCoord = ParallaxMapping(VertexInput.TexCoord, viewDirection);
	}

	if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
	{
		discard;
	}

	return texCoord;
}

vec4 GetAlbedoSpecularOutput(vec2 texCoord)
{
	vec4 diffuseColor = texture(u_Textures[v_DiffuseTextureIndex] , texCoord);

	if (diffuseColor.a == 0.0)
	{
		discard;
	}

	diffuseColor = vec4(pow(diffuseColor.rgb, vec3(u_Gamma)) * pow(VertexInput.DiffuseColor.rgb, vec3(u_Gamma)), diffuseColor.a);

	float specularFactor = texture(u_Textures[v_SpecularTextureIndex], texCoord).r;
	diffuseColor.a = specularFactor;

	return diffuseColor;
}

vec4 GetNormalShininessOutput(vec2 texCoord)
{
	vec3 vertexNormal;

	if(v_NormalMapTextureIndex == 0)
	{
		vertexNormal = normalize(VertexInput.Normal);
	}
	else
	{
		vec3 normalMap = texture(u_Textures[v_NormalMapTextureIndex], texCoord).rgb;
		vertexNormal = normalMap * 2.0 - 1.0;
		vertexNormal = normalize(VertexInput.TBN * vertexNormal);
	}

	return vec4(vertexNormal, VertexInput.Shininess);
}