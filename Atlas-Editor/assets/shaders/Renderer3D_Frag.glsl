//--------------------------
// - Atlas 3D -
// Renderer 3D Fragment Shader
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

layout (location = 0) in VertexData VertexInput;
layout (location = 7) in flat uint  v_DiffuseTextureIndex;
layout (location = 8) in flat uint  v_SpecularTextureIndex;
layout (location = 9) in flat int   v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_CameraPosition;
};

layout(std140, binding = 1) uniform AmbientLight
{
	uint  u_LightCount;
	vec3  u_AmbientLightColor;
	float u_AmbientLightIntensity;
};

layout(std430, binding = 2) buffer LightPositions
{
	vec3 u_LightPositions[];
};

layout(std430, binding = 3) buffer LightColors
{
	vec3 u_LightColors[];
};

layout(std430, binding = 4) buffer LightDirections
{
	vec4 u_LightDirections[];
};

layout(std430, binding = 5) buffer LightRadius
{
	float u_LightRadius[];
};

layout(std430, binding = 6) buffer LightIntensities
{
	float u_LightIntensities[];
};

layout(std430, binding = 7) buffer LightCutOffs
{
	vec2 u_LightCutOffs[];
};

layout(std430, binding = 8) buffer LightAmbientStrengths
{
	float u_LightAmbientStrengths[];
};

layout(std430, binding = 9) buffer LightDiffuseStrengths
{
	float u_LightDiffuseStrengths[];
};

layout(std430, binding = 10) buffer LightSpecularStrengths
{
	float u_LightSpecularStrengths[];
};

layout(location = 0) out vec4 o_color;
layout(location = 1) out int  o_entityID;

vec3 CalculateAmbientLight(vec3 lightColor, float lightAmbientStrength)
{
	return lightColor * lightAmbientStrength * VertexInput.AmbientColor;
}

vec3 CalculateDiffuseLight(vec3 lightColor, float lightDiffuseStrength, vec3 lightDirection, vec3 vertexNormal)
{
	float diffuseImpact = max(dot(vertexNormal, lightDirection), 0.0);
	return lightColor * lightDiffuseStrength * diffuseImpact * VertexInput.DiffuseColor;
}

vec3 CalculateSpecularLight(vec3 lightColor, float lightSpecularStrength, vec3 lightDirection, vec3 vertexNormal)
{
	vec3 viewDirection       = normalize(u_CameraPosition - VertexInput.Position);
	vec3 reflectionDirection = reflect(-lightDirection, vertexNormal);
	float specularFactor     = pow(max(dot(viewDirection, reflectionDirection), 0.0), VertexInput.Shininess == 0 ? 1 : VertexInput.Shininess * 128); // TODO: Fix Shininess == 0
	return lightColor * lightSpecularStrength * (specularFactor * VertexInput.SpecularColor);
}

float CalculateLightAttenuation(float lightRadius, vec3 lightPosition, vec3 vertexPosition)
{
	float attenuation = 1.0;

	if(lightRadius > 0)
	{
		float dist  = length(lightPosition - vertexPosition);

		// Cem Yuksel Nonsingular Point Light Attenuation
		float temp  = dist * dist + lightRadius * lightRadius;
		attenuation = 2.0 / (temp + dist * sqrt(temp));
		//attenuation = 1.0 / temp;
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

vec4 CalculateLights(vec4 diffuseTexture, vec4 specularTexture)
{
	vec4  ambientColor  = vec4(0.0, 0.0, 0.0, 1.0);
	vec4  diffuseColor  = vec4(0.0, 0.0, 0.0, 1.0);
	vec4  specularColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 vertexNormal = normalize(VertexInput.Normal);

	for (uint lightIndex = 0; lightIndex < u_LightCount; lightIndex++)
	{
		vec3 lightColor = u_LightColors[lightIndex] * u_LightIntensities[lightIndex];

		vec3 lightDirection;
		if(u_LightDirections[lightIndex].w == 0)
		{
			lightDirection = normalize(u_LightPositions[lightIndex] - VertexInput.Position);
		}
		else
		{
			lightDirection = normalize(-u_LightDirections[lightIndex].xyz); // For directional light ; light direction is spot direction
		}
		
		float attenuation = CalculateLightAttenuation(u_LightRadius[lightIndex], u_LightPositions[lightIndex], VertexInput.Position);
		float lightCutOff = CalculateLightCutOff(u_LightCutOffs[lightIndex], normalize(u_LightPositions[lightIndex] - VertexInput.Position), u_LightDirections[lightIndex].xyz);

		vec4 ambientLight  = vec4(CalculateAmbientLight  (lightColor, u_LightAmbientStrengths [lightIndex]                              ) * attenuation              , 1.0);
		vec4 diffuseLight  = vec4(CalculateDiffuseLight  (lightColor, u_LightDiffuseStrengths [lightIndex], lightDirection, vertexNormal) * attenuation * lightCutOff, 1.0);
		vec4 specularLight = vec4(CalculateSpecularLight (lightColor, u_LightSpecularStrengths[lightIndex], lightDirection, vertexNormal) * attenuation * lightCutOff, 1.0);
		ambientLight  *= diffuseTexture;
		diffuseLight  *= diffuseTexture;
		specularLight *= specularTexture;

		ambientColor  += ambientLight;
		diffuseColor  += diffuseLight;
		specularColor += specularLight;
	}

	return ambientColor + diffuseColor + specularColor;
}

vec4 GetTexture(uint textureIndex)
{
	switch(textureIndex)
	{
		case  0: return texture(u_Textures[ 0], VertexInput.TexCoord);
		case  1: return texture(u_Textures[ 1], VertexInput.TexCoord);
		case  2: return texture(u_Textures[ 2], VertexInput.TexCoord);
		case  3: return texture(u_Textures[ 3], VertexInput.TexCoord);
		case  4: return texture(u_Textures[ 4], VertexInput.TexCoord);
		case  5: return texture(u_Textures[ 5], VertexInput.TexCoord);
		case  6: return texture(u_Textures[ 6], VertexInput.TexCoord);
		case  7: return texture(u_Textures[ 7], VertexInput.TexCoord);
		case  8: return texture(u_Textures[ 8], VertexInput.TexCoord);
		case  9: return texture(u_Textures[ 9], VertexInput.TexCoord);
		case 10: return texture(u_Textures[10], VertexInput.TexCoord);
		case 11: return texture(u_Textures[11], VertexInput.TexCoord);
		case 12: return texture(u_Textures[12], VertexInput.TexCoord);
		case 13: return texture(u_Textures[13], VertexInput.TexCoord);
		case 14: return texture(u_Textures[14], VertexInput.TexCoord);
		case 15: return texture(u_Textures[15], VertexInput.TexCoord);
		case 16: return texture(u_Textures[16], VertexInput.TexCoord);
		case 17: return texture(u_Textures[17], VertexInput.TexCoord);
		case 18: return texture(u_Textures[18], VertexInput.TexCoord);
		case 19: return texture(u_Textures[19], VertexInput.TexCoord);
		case 20: return texture(u_Textures[20], VertexInput.TexCoord);
		case 21: return texture(u_Textures[21], VertexInput.TexCoord);
		case 22: return texture(u_Textures[22], VertexInput.TexCoord);
		case 23: return texture(u_Textures[23], VertexInput.TexCoord);
		case 24: return texture(u_Textures[24], VertexInput.TexCoord);
		case 25: return texture(u_Textures[25], VertexInput.TexCoord);
		case 26: return texture(u_Textures[26], VertexInput.TexCoord);
		case 27: return texture(u_Textures[27], VertexInput.TexCoord);
		case 28: return texture(u_Textures[28], VertexInput.TexCoord);
		case 29: return texture(u_Textures[29], VertexInput.TexCoord);
		case 30: return texture(u_Textures[30], VertexInput.TexCoord);
		case 31: return texture(u_Textures[31], VertexInput.TexCoord);
	}
}

void main()
{
	vec4 diffuseTexture  = GetTexture(v_DiffuseTextureIndex);
	vec4 specularTexture = GetTexture(v_SpecularTextureIndex);

	// Alpha discard on diffuse
	if (diffuseTexture.a == 0.0)
	{
		discard;
	}

	vec4 fragmentColor = CalculateLights(diffuseTexture, specularTexture);

	// Color buffer
	o_color = fragmentColor;

	// Entity ID buffer
	o_entityID = v_EntityID;
}