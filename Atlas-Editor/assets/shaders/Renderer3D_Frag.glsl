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
layout (location = 7) in flat int   v_DiffuseTextureIndex;
layout (location = 8) in flat int   v_EntityID;

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

layout(std430, binding = 4) buffer LightIntensities
{
	float u_LightIntensities[];
};

layout(std430, binding = 5) buffer LightAmbientStrengths
{
	float u_LightAmbientStrengths[];
};

layout(std430, binding = 6) buffer LightDiffuseStrengths
{
	float u_LightDiffuseStrengths[];
};

layout(std430, binding = 7) buffer LightSpecularStrengths
{
	float u_LightSpecularStrengths[];
};

layout(location = 0) out vec4 o_color;
layout(location = 1) out int  o_entityID;

vec4 GetAmbientColor(vec3 lightColor, float lightAmbientStrength)
{
	return vec4(lightColor * lightAmbientStrength * VertexInput.AmbientColor, 1.0);
}

vec4 GetDiffuseColor(vec3 lightColor, float lightDiffuseStrength, vec3 lightDirection, vec3 vertexNormal)
{
	float diffuseImpact = max(dot(vertexNormal, lightDirection), 0.0);
	return vec4(lightColor * lightDiffuseStrength * diffuseImpact * VertexInput.DiffuseColor, 1.0);
}

vec4 GetSpecularColor(vec3 lightColor, float lightSpecularStrength, vec3 lightDirection, vec3 vertexNormal)
{
	vec3 viewDirection       = normalize(u_CameraPosition - VertexInput.Position);
	vec3 reflectionDirection = reflect(-lightDirection, vertexNormal);
	float specularFactor     = pow(max(dot(viewDirection, reflectionDirection), 0.0), VertexInput.Shininess == 0 ? 1 : VertexInput.Shininess * 128); // TODO: Fix Shininess == 0
	return vec4(lightColor * lightSpecularStrength * (specularFactor * VertexInput.SpecularColor), 1.0);
}

vec4 GetColorFromLights(vec4 diffuseTextureColor)
{
	// Ambient, diffuse & specular lighting
	// TODO: Fix ambient light settings not working (color corresponds to intensity and intensity is null)
	//vec3  ambientColor  = u_AmbientLightColor * u_AmbientLightIntensity;
	vec4  ambientColor  = vec4(0.0);
	vec4  diffuseColor  = vec4(0.0);
	vec4  specularColor = vec4(0.0);

	vec3 vertexNormal = normalize(VertexInput.Normal);

	for (uint lightIndex = 0; lightIndex < u_LightCount; lightIndex++)
	{
		vec3 lightColor     = u_LightColors[lightIndex] * u_LightIntensities[lightIndex];
		vec3 lightDirection = normalize(u_LightPositions[lightIndex] - VertexInput.Position);
		
		ambientColor  += GetAmbientColor  (lightColor, u_LightAmbientStrengths [lightIndex]                              ) * diffuseTextureColor;
		diffuseColor  += GetDiffuseColor  (lightColor, u_LightDiffuseStrengths [lightIndex], lightDirection, vertexNormal) * diffuseTextureColor;
		specularColor += GetSpecularColor (lightColor, u_LightSpecularStrengths[lightIndex], lightDirection, vertexNormal);
	}

	return ambientColor + diffuseColor + specularColor;
}

void main()
{
	vec4 texColor = vec4(1.0);

	switch(v_DiffuseTextureIndex)
	{
		case  0: texColor *= texture(u_Textures[ 0], VertexInput.TexCoord); break;
		case  1: texColor *= texture(u_Textures[ 1], VertexInput.TexCoord); break;
		case  2: texColor *= texture(u_Textures[ 2], VertexInput.TexCoord); break;
		case  3: texColor *= texture(u_Textures[ 3], VertexInput.TexCoord); break;
		case  4: texColor *= texture(u_Textures[ 4], VertexInput.TexCoord); break;
		case  5: texColor *= texture(u_Textures[ 5], VertexInput.TexCoord); break;
		case  6: texColor *= texture(u_Textures[ 6], VertexInput.TexCoord); break;
		case  7: texColor *= texture(u_Textures[ 7], VertexInput.TexCoord); break;
		case  8: texColor *= texture(u_Textures[ 8], VertexInput.TexCoord); break;
		case  9: texColor *= texture(u_Textures[ 9], VertexInput.TexCoord); break;
		case 10: texColor *= texture(u_Textures[10], VertexInput.TexCoord); break;
		case 11: texColor *= texture(u_Textures[11], VertexInput.TexCoord); break;
		case 12: texColor *= texture(u_Textures[12], VertexInput.TexCoord); break;
		case 13: texColor *= texture(u_Textures[13], VertexInput.TexCoord); break;
		case 14: texColor *= texture(u_Textures[14], VertexInput.TexCoord); break;
		case 15: texColor *= texture(u_Textures[15], VertexInput.TexCoord); break;
		case 16: texColor *= texture(u_Textures[16], VertexInput.TexCoord); break;
		case 17: texColor *= texture(u_Textures[17], VertexInput.TexCoord); break;
		case 18: texColor *= texture(u_Textures[18], VertexInput.TexCoord); break;
		case 19: texColor *= texture(u_Textures[19], VertexInput.TexCoord); break;
		case 20: texColor *= texture(u_Textures[20], VertexInput.TexCoord); break;
		case 21: texColor *= texture(u_Textures[21], VertexInput.TexCoord); break;
		case 22: texColor *= texture(u_Textures[22], VertexInput.TexCoord); break;
		case 23: texColor *= texture(u_Textures[23], VertexInput.TexCoord); break;
		case 24: texColor *= texture(u_Textures[24], VertexInput.TexCoord); break;
		case 25: texColor *= texture(u_Textures[25], VertexInput.TexCoord); break;
		case 26: texColor *= texture(u_Textures[26], VertexInput.TexCoord); break;
		case 27: texColor *= texture(u_Textures[27], VertexInput.TexCoord); break;
		case 28: texColor *= texture(u_Textures[28], VertexInput.TexCoord); break;
		case 29: texColor *= texture(u_Textures[29], VertexInput.TexCoord); break;
		case 30: texColor *= texture(u_Textures[30], VertexInput.TexCoord); break;
		case 31: texColor *= texture(u_Textures[31], VertexInput.TexCoord); break;
		}

	// Alpha discard
	if (texColor.a == 0.0)
	{
		discard;
	}

	// Color buffer
	o_color = GetColorFromLights(texColor);

	// Entity ID buffer
	o_entityID = v_EntityID;
}