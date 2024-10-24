//--------------------------
// - Atlas 3D -
// Renderer 3D Fragment Shader
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
};

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec4 u_CameraPosition;
};

layout (std140, binding = 2) uniform LightCount
{
	uint u_LightCount;
};

layout (std430, binding = 0) buffer Lights
{
	LightData u_Lights[];
};

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;
layout (location = 2) out vec4 o_PostProcessColor;

/* ------------------------------ */
/* ----- METHOD DEFINITIONS ----- */
/* ------------------------------ */

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDirection);
vec4 CalculateLights(vec4 diffuseTexture, vec4 specularTexture, vec3 vertexNormal);

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	vec2 texCoord = VertexInput.TexCoord;

	// Parallax Mapping
	if(v_HeightMapTextureIndex != 0)
	{
		vec3 viewDirection = normalize(VertexInput.TBN * u_CameraPosition.xyz - VertexInput.TBN * VertexInput.Position);
		texCoord = ParallaxMapping(VertexInput.TexCoord, viewDirection);
	}

	if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
	{
		discard;
	}

	vec4 diffuseColor  = texture(u_Textures[v_DiffuseTextureIndex] , texCoord);
	vec4 specularColor = texture(u_Textures[v_SpecularTextureIndex], texCoord);

	diffuseColor = vec4(pow(diffuseColor.rgb, vec3(u_Gamma)), diffuseColor.a);

	// Alpha discard on diffuse
	if (diffuseColor.a == 0.0)
	{
		discard;
	}

	vec3 vertexNormal;
	if(v_NormalMapTextureIndex == 0)
	{
		vertexNormal = normalize(VertexInput.Normal);
	}
	else
	{
		vec3 normalMap = texture(u_Textures[v_NormalMapTextureIndex] , texCoord).rgb;
		vertexNormal = normalMap * 2.0 - 1.0;
		vertexNormal = normalize(VertexInput.TBN * vertexNormal);
	}

	vec4 fragmentColor = CalculateLights(diffuseColor, specularColor, vertexNormal);

	o_Color            = fragmentColor;
	o_EntityID         = v_EntityID;
	o_PostProcessColor = o_Color;
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

vec3 CalculateAmbientLight(vec3 lightColor, float lightAmbientStrength)
{
	return lightColor * lightAmbientStrength * pow(VertexInput.AmbientColor.rgb, vec3(u_Gamma));
}

vec3 CalculateDiffuseLight(vec3 lightColor, float lightDiffuseStrength, vec3 lightDirection, vec3 vertexNormal)
{
	float diffuseImpact = max(dot(vertexNormal, lightDirection), 0.0);
	return lightColor * lightDiffuseStrength * diffuseImpact * pow(VertexInput.DiffuseColor.rgb, vec3(u_Gamma));
}

vec3 CalculateSpecularLight(vec3 lightColor, float lightSpecularStrength, vec3 lightDirection, vec3 vertexNormal)
{
	vec3 viewDirection  = normalize(u_CameraPosition.xyz - VertexInput.Position);
	
	/* --- Phong Specular Lighting  --- */ 
	//vec3 reflectionDirection = reflect(-lightDirection, vertexNormal);
	//float specularFactor     = pow(max(dot(viewDirection, reflectionDirection), 0.0), VertexInput.Shininess == 0 ? 1 : VertexInput.Shininess * 128); // TODO: Fix Shininess == 0

	/* --- Blinn-Phong Specular Lighting  --- */
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularFactor  = pow(max(dot(vertexNormal, halfwayDirection), 0.0), VertexInput.Shininess * 128);

	return lightColor * lightSpecularStrength * (specularFactor * pow(VertexInput.SpecularColor.rgb, vec3(u_Gamma)));
}

float CalculateLightAttenuation(float lightRadius, vec3 lightPosition, vec3 vertexPosition)
{
	float attenuation = 1.0;

	if(lightRadius > 0)
	{
		float dist  = length(lightPosition - vertexPosition);

		float temp  = dist * dist + lightRadius * lightRadius;
		attenuation = 2.0 / (temp + dist * sqrt(temp));
	}

	return attenuation;
}

float CalculateLightCutOff(vec2 lightCutOff, vec3 lightDirection, vec3 spotDirection)
{
	float cutOff = 1.0f;

	if(lightCutOff.x >= 0 && lightCutOff.y >= 0)
	{
		float theta   = dot(lightDirection, normalize(-spotDirection));
		float epsilon = lightCutOff.x - lightCutOff.y;
		cutOff        = clamp((theta - lightCutOff.y) / epsilon, 0.0, 1.0);
	}

	return cutOff;
}

vec4 CalculateLights(vec4 diffuseTexture, vec4 specularTexture, vec3 vertexNormal)
{
	vec4  ambientColor  = vec4(0.0, 0.0, 0.0, 1.0);
	vec4  diffuseColor  = vec4(0.0, 0.0, 0.0, 1.0);
	vec4  specularColor = vec4(0.0, 0.0, 0.0, 1.0);

	for (uint lightIndex = 0; lightIndex < u_LightCount; lightIndex++)
	{
		LightData light = u_Lights[lightIndex];

		vec3 lightColor = pow(light.Color.rgb, vec3(u_Gamma)) * light.Intensity;

		vec3 lightDirection;
		if(light.Direction.w == 0)
		{
			lightDirection = normalize(light.Position.xyz - VertexInput.Position);
		}
		else
		{
			lightDirection = normalize(-light.Direction.xyz); // For directional light ; light direction is spot direction
		}
		
		float attenuation = CalculateLightAttenuation(light.Radius, light.Position.xyz, VertexInput.Position);
		float lightCutOff = CalculateLightCutOff(light.CutOffs, normalize(light.Position.xyz - VertexInput.Position), light.Direction.xyz);

		vec4 ambientLight  = vec4(CalculateAmbientLight  (lightColor, light.AmbientStrength                               ) * attenuation              , 1.0);
		vec4 diffuseLight  = vec4(CalculateDiffuseLight  (lightColor, light.DiffuseStrength , lightDirection, vertexNormal) * attenuation * lightCutOff, 1.0);
		vec4 specularLight = vec4(CalculateSpecularLight (lightColor, light.SpecularStrength, lightDirection, vertexNormal) * attenuation * lightCutOff, 1.0);
		ambientLight  *= diffuseTexture;
		diffuseLight  *= diffuseTexture;
		specularLight *= specularTexture;

		ambientColor  += ambientLight;
		diffuseColor  += diffuseLight;
		specularColor += specularLight;
	}

	return ambientColor + diffuseColor + specularColor;
}