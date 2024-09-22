//--------------------------
// - Atlas 3D -
// Renderer3D Flat Color Shader
// --------------------------

#type vertex
#version 450 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec3  a_Normal;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in vec3  a_AmbientColor;
layout(location = 6) in vec3  a_DiffuseColor;
layout(location = 7) in vec3  a_SpecularColor;
layout(location = 8) in float a_Shininess;
layout(location = 9) in int   a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_CameraPosition;
};

struct VertexData
{
	vec3  Position;
	vec3  Normal;

	vec2  TexCoord;
	float TilingFactor;

	vec3  AmbientColor;
	vec3  DiffuseColor;
	vec3  SpecularColor;
	float Shininess;
};

layout (location = 0) out VertexData VertexOutput;
layout (location = 8) out flat float v_TexIndex;
layout (location = 9) out flat int   v_EntityID;

void main()
{
	VertexOutput.Position      = a_Position;
	VertexOutput.Normal        = a_Normal;

	VertexOutput.TexCoord      = a_TexCoord;
	VertexOutput.TilingFactor  = a_TilingFactor;

	VertexOutput.AmbientColor  = a_AmbientColor;
	VertexOutput.DiffuseColor  = a_DiffuseColor;
	VertexOutput.SpecularColor = a_SpecularColor;
	VertexOutput.Shininess     = a_Shininess;

	v_TexIndex                 = a_TexIndex;
	v_EntityID                 = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

struct VertexData
{
	vec3  Position;
	vec3  Normal;
	vec2  TexCoord;
	float TilingFactor;

	vec3  AmbientColor;
	vec3  DiffuseColor;
	vec3  SpecularColor;
	float Shininess;
};

layout (location = 0) in VertexData VertexInput;
layout (location = 8) in flat float v_TexIndex;
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

vec4 GetLightColor()
{
	// Ambient, diffuse & specular lighting
	// TODO: Fix ambient light settings not working (color corresponds to intensity and intensity is null)
	//vec3  ambientColor  = u_AmbientLightColor * u_AmbientLightIntensity;
	vec3  ambientColor  = vec3(0.0);
	vec3  diffuseColor  = vec3(0.0);
	vec3  specularColor = vec3(0.0);

	vec3 norm = normalize(VertexInput.Normal);

	for (uint i = 0; i < u_LightCount; i++)
	{
		vec3 lightColor = u_LightColors[i] * u_LightIntensities[i];
		
		ambientColor += (lightColor * u_LightAmbientStrengths[i]) * VertexInput.AmbientColor;

		vec3 lightDirection = normalize(u_LightPositions[i] - VertexInput.Position);
		float diffuseImpact = max(dot(norm, lightDirection), 0.0);
		diffuseColor += (lightColor * u_LightDiffuseStrengths[i]) * (diffuseImpact * VertexInput.DiffuseColor);

		vec3 viewDirection       = normalize(u_CameraPosition - VertexInput.Position);
		vec3 reflectionDirection = reflect(-lightDirection, norm);
		float specularFactor     = pow(max(dot(viewDirection, reflectionDirection), 0.0), VertexInput.Shininess == 0 ? 1 : VertexInput.Shininess * 128);
		specularColor += (lightColor * u_LightSpecularStrengths[i]) * (specularFactor * VertexInput.SpecularColor);
	}

	return vec4(ambientColor + diffuseColor + specularColor, 1.0);
}

vec4 GetTextureColor()
{
	vec4 texColor = vec4(1.0);

	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 16: texColor *= texture(u_Textures[16], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 17: texColor *= texture(u_Textures[17], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 18: texColor *= texture(u_Textures[18], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 19: texColor *= texture(u_Textures[19], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 20: texColor *= texture(u_Textures[20], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 21: texColor *= texture(u_Textures[21], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 22: texColor *= texture(u_Textures[22], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 23: texColor *= texture(u_Textures[23], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 24: texColor *= texture(u_Textures[24], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 25: texColor *= texture(u_Textures[25], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 26: texColor *= texture(u_Textures[26], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 27: texColor *= texture(u_Textures[27], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 28: texColor *= texture(u_Textures[28], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 29: texColor *= texture(u_Textures[29], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 30: texColor *= texture(u_Textures[30], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		case 31: texColor *= texture(u_Textures[31], VertexInput.TexCoord * VertexInput.TilingFactor); break;
		}

	// Alpha discard
	if (texColor.a == 0.0)
	{
		discard;
	}

	return texColor;
}

void main()
{
	// Color buffer
	o_color = GetTextureColor() * GetLightColor();

	// Entity ID buffer
	o_entityID = v_EntityID;
}