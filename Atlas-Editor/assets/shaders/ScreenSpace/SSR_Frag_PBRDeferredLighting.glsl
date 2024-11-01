//--------------------------
// - Atlas Screen-Space Renderer -
// PBR Deferred Lighting Fragment Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- STRUCTS ---------- */
/* ------------------------------ */

struct LightData
{
	vec4 Position;
	vec4 Color;
	vec4 Direction; // w is a flag to indicate if light direction is spot direction

	float Intensity;
	vec2 CutOffs; // (inner, outer); negative value means cutoff is disabled
};

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in vec2 v_TexCoords;

// 0: Position
// 1: Normal
// 2: Albedo
// 3: X: Metallic, Y: Roughness, Z: AO
// 4: R(1-Channel Texture): SSAO
layout (binding = 0) uniform sampler2D u_ScreenTextures[5];

layout (binding = 5) uniform sampler2D u_BRDFLUT; 
layout (binding = 6) uniform samplerCube u_IBLMaps[2];

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
layout (location = 1) out vec4 o_BrightColor;

/* ------------------------------ */
/* ----- METHOD DEFINITIONS ----- */
/* ------------------------------ */

vec3 CalculateColor(vec3 vertexPosition, vec3 vertexNormal, vec3 albedo, float metallic, float roughness, float ambientOcclusion);
vec4 CalculateBrightColor(vec3 fragmentColor);

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

const float PI = 3.14159265359;

void main()
{
	vec3  vertexPosition   = texture(u_ScreenTextures[0], v_TexCoords).rgb;
	vec3  vertexNormal     = texture(u_ScreenTextures[1], v_TexCoords).rgb;
	vec3  albedo           = texture(u_ScreenTextures[2], v_TexCoords).rgb;
	float metallic         = texture(u_ScreenTextures[3], v_TexCoords).x;
	float roughness        = texture(u_ScreenTextures[3], v_TexCoords).y;
	float ambientOcclusion = texture(u_ScreenTextures[3], v_TexCoords).z + texture(u_ScreenTextures[4], v_TexCoords).r;

	vec3 fragmentColor = CalculateColor(vertexPosition, vertexNormal, albedo, metallic, roughness, ambientOcclusion);

	o_Color = vec4(fragmentColor.rgb, 1.0);
	o_BrightColor = CalculateBrightColor(fragmentColor);
}

/* ------------------------------ */
/* --- METHOD IMPLEMENTATIONS --- */
/* ------------------------------ */

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float CalculateLightAttenuation(vec3 vertexPosition, vec3 lightPosition, vec4 lightDirection)
{
	float attenuation = 1.0;

	// Directional lights have no attenuation
	if(lightDirection.w == 0)
	{
		float dist  = length(lightPosition - vertexPosition);
		attenuation = 1.0 / (dist * dist);
	}

	return attenuation;
}

float CalculateLightCutOff(vec2 lightCutOff, vec3 lightDirection, vec3 spotDirection)
{
	float cutOff = 1.0;

	// Only spotlights have cut-offs
	if(lightCutOff.x >= 0 && lightCutOff.y >= 0)
	{
		float theta   = dot(lightDirection, normalize(-spotDirection));
		float epsilon = lightCutOff.x - lightCutOff.y;
		cutOff        = clamp((theta - lightCutOff.y) / epsilon, 0.0, 1.0);
	}

	return cutOff;
}

vec3 CalculateColor(vec3 vertexPosition, vec3 vertexNormal, vec3 albedo, float metallic, float roughness, float ambientOcclusion)
{
	vec3 Lo = vec3(0.0);
	
	vec3 viewDirection  = normalize(u_CameraPosition.xyz - vertexPosition);
	vec3 R = reflect(-viewDirection, vertexNormal); 

	vec3 F0 = vec3(0.04);
		 F0 = mix(F0, albedo, metallic);

	for (uint lightIndex = 0; lightIndex < u_LightCount; lightIndex++)
	{
		LightData light = u_Lights[lightIndex];
		vec3 radiance = pow(light.Color.rgb, vec3(u_Gamma)) * light.Intensity
			* CalculateLightAttenuation(vertexPosition, light.Position.xyz, light.Direction)
			* CalculateLightCutOff(light.CutOffs, normalize(light.Position.xyz - vertexPosition), light.Direction.xyz);

		vec3 lightDirection = normalize(light.Position.xyz - vertexPosition);
		if(light.Direction.w != 0)
		{
			lightDirection = normalize(-light.Direction.xyz); // Directional lights have no position
		}

		vec3 H = normalize(viewDirection + lightDirection);

		vec3 F  = fresnelSchlick(max(dot(H, viewDirection), 0.0), F0);

		float NDF = DistributionGGX(vertexNormal, H, roughness);
		float G   = GeometrySmith(vertexNormal, viewDirection, lightDirection, roughness);

		vec3 numerator    = NDF * G * F;
		float denominator = 4.0 * max(dot(vertexNormal, viewDirection), 0.0) * max(dot(vertexNormal, lightDirection), 0.0) + 0.0001;
		vec3 specular     = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
  
		kD *= 1.0 - metallic;

		float NdotL = max(dot(vertexNormal, lightDirection), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 F = fresnelSchlickRoughness(max(dot(vertexNormal, viewDirection), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(u_IBLMaps[0], vertexNormal).rgb;
    vec3 diffuse    = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_IBLMaps[1], R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(u_BRDFLUT, vec2(max(dot(vertexNormal, viewDirection), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ambientOcclusion;

	return ambient + Lo;
}

vec4 CalculateBrightColor(vec3 fragmentColor)
{
	vec4 brightColor = vec4(0.0, 0.0, 0.0, 1.0);

	float brightness = dot(fragmentColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > u_BloomThreshold)
	{
        brightColor = vec4(fragmentColor.rgb, 1.0);
	}

	return brightColor;
}
