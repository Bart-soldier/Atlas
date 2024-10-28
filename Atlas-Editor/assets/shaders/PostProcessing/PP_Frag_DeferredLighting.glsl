//--------------------------
// - Atlas 3D -
// Renderer 3D Deferred Lighting Fragment Shader
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

layout (location = 0) in vec2 v_TexCoords;

// 0: Position
// 1: Normal
// 2: Albedo
// 3: RGB: Specular, A: Shininess
// 4: R(1-Channel Texture): SSAO
layout (binding = 0) uniform sampler2D u_ScreenTextures[5];

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

vec3 CalculateLights(vec3 vertexPosition, vec3 vertexNormal, vec3 albedo, vec3 specular, float shininess, float ambientOcclusion);

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	vec3 position          = texture(u_ScreenTextures[0], v_TexCoords).rgb;
	vec3 normal            = texture(u_ScreenTextures[1], v_TexCoords).rgb;
	vec3 albedo            = texture(u_ScreenTextures[2], v_TexCoords).rgb;
	vec3 specular          = texture(u_ScreenTextures[3], v_TexCoords).rgb;
	float shininess        = texture(u_ScreenTextures[3], v_TexCoords).a;
	float ambientOcclusion = texture(u_ScreenTextures[4], v_TexCoords).r;

	vec3 fragmentColor = CalculateLights(position, normal, albedo, specular, shininess, ambientOcclusion);

	float brightness = dot(fragmentColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	vec4 brightColor;
	if(brightness > u_BloomThreshold)
	{
        brightColor = vec4(fragmentColor.rgb, 1.0);
	}
    else
	{
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}

	o_Color = vec4(fragmentColor.rgb, 1.0);
	o_BrightColor = brightColor;
}

/* ------------------------------ */
/* --- METHOD IMPLEMENTATIONS --- */
/* ------------------------------ */

vec3 CalculateAmbientLight(vec3 albedo, vec3 lightColor, float lightAmbientStrength, float ambientOcclusion)
{
	return lightColor * lightAmbientStrength * albedo * ambientOcclusion;
}

vec3 CalculateDiffuseLight(vec3 albedo, vec3 lightColor, float lightDiffuseStrength, vec3 lightDirection, vec3 vertexNormal)
{
	float diffuseImpact = max(dot(vertexNormal, lightDirection), 0.0);
	return lightColor * lightDiffuseStrength * diffuseImpact * albedo;
}

vec3 CalculateSpecularLight(vec3 specular, vec3 lightColor, float lightSpecularStrength, vec3 lightDirection, vec3 vertexNormal, vec3 vertexPosition, float shininess)
{
	vec3 viewDirection  = normalize(u_CameraPosition.xyz - vertexPosition);
	
	/* --- Phong Specular Lighting  --- */ 
	//vec3 reflectionDirection = reflect(-lightDirection, vertexNormal);
	//float specularFactor     = pow(max(dot(viewDirection, reflectionDirection), 0.0), VertexInput.Shininess == 0 ? 1 : VertexInput.Shininess * 128); // TODO: Fix Shininess == 0

	/* --- Blinn-Phong Specular Lighting  --- */
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularFactor  = pow(max(dot(vertexNormal, halfwayDirection), 0.0), shininess * 128);

	return lightColor * lightSpecularStrength * (specularFactor * specular);
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

vec3 CalculateLights(vec3 vertexPosition, vec3 vertexNormal, vec3 albedo, vec3 specular, float shininess, float ambientOcclusion)
{
	vec3 ambientColor  = vec3(0.0);
	vec3 diffuseColor  = vec3(0.0);
	vec3 specularColor = vec3(0.0);

	for (uint lightIndex = 0; lightIndex < u_LightCount; lightIndex++)
	{
		LightData light = u_Lights[lightIndex];

		vec3 lightColor = pow(light.Color.rgb, vec3(u_Gamma)) * light.Intensity;

		vec3 lightDirection;
		if(light.Direction.w == 0)
		{
			lightDirection = normalize(light.Position.xyz - vertexPosition);
		}
		else
		{
			lightDirection = normalize(-light.Direction.xyz); // For directional light ; light direction is spot direction
		}
		
		float attenuation = CalculateLightAttenuation(light.Radius, light.Position.xyz, vertexPosition);
		float lightCutOff = CalculateLightCutOff(light.CutOffs, normalize(light.Position.xyz - vertexPosition), light.Direction.xyz);

		vec3 ambientLight  = CalculateAmbientLight (albedo,   lightColor, light.AmbientStrength                                               , ambientOcclusion);
		vec3 diffuseLight  = CalculateDiffuseLight (albedo,   lightColor, light.DiffuseStrength , lightDirection, vertexNormal                                  );
		vec3 specularLight = CalculateSpecularLight(specular, lightColor, light.SpecularStrength, lightDirection, vertexNormal, vertexPosition, shininess       );
		ambientLight  *= attenuation;
		diffuseLight  *= attenuation * lightCutOff;
		specularLight *= attenuation * lightCutOff;

		ambientColor  += ambientLight;
		diffuseColor  += diffuseLight;
		specularColor += specularLight;
	}

	//return ambientColor + diffuseColor + specularColor;
	return ambientColor;
}