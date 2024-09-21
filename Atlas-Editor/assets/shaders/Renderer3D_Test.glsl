//--------------------------
// - Atlas 3D -
// Renderer3D Flat Color Shader
// --------------------------

#type vertex
#version 450 core

layout(location = 0)  in vec3  a_Position;
layout(location = 1)  in vec3  a_Normal;
layout(location = 2)  in vec2  a_TexCoord;
layout(location = 3)  in float a_TexIndex;
layout(location = 4)  in float a_TilingFactor;
layout(location = 5)  in vec4  a_Color;
layout(location = 6)  in vec3  a_AmbientTint;
layout(location = 7)  in vec3  a_DiffuseTint;
layout(location = 8)  in vec3  a_SpecularTint;
layout(location = 9)  in float a_Shininess;
layout(location = 10) in int   a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
	vec3 u_CameraPosition;
};

struct VertexOutput
{
	vec3  Position;
	vec3  Normal;

	vec2  TexCoord;
	float TilingFactor;

	vec4  Color;
	vec3  AmbientTint;
	vec3  DiffuseTint;
	vec3  SpecularTint;
	float Shininess;
};

layout (location = 0)  out VertexOutput Output;
layout (location = 9)  out flat float v_TexIndex;
layout (location = 10) out flat int v_EntityID;

void main()
{
	Output.Position     = a_Position;
	Output.Normal       = a_Normal;

	Output.TexCoord     = a_TexCoord;
	Output.TilingFactor = a_TilingFactor;

	Output.Color        = a_Color;
	Output.AmbientTint  = a_AmbientTint;
	Output.DiffuseTint  = a_DiffuseTint;
	Output.SpecularTint = a_SpecularTint;
	Output.Shininess    = a_Shininess;

	v_TexIndex          = a_TexIndex;
	v_EntityID          = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec3  Position;
	vec3  Normal;
	vec2  TexCoord;
	float TilingFactor;

	vec4  Color;
	vec3  AmbientTint;
	vec3  DiffuseTint;
	vec3  SpecularTint;
	float Shininess;
};

layout (location = 0)  in VertexOutput Input;
layout (location = 9)  in flat float   v_TexIndex;
layout (location = 10) in flat int     v_EntityID;

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
	vec3  ambientTint     = u_AmbientLightColor * u_AmbientLightIntensity;
	vec3  diffuseTint     = vec3(0.0);
	vec3  specularTint    = vec3(0.0);

	vec3 norm = normalize(Input.Normal);

	for (uint i = 0; i < u_LightCount; i++)
	{
		vec3 lightColor = u_LightColors[i] * u_LightIntensities[i];
		
		ambientTint *= (lightColor * u_LightAmbientStrengths[i]) * Input.AmbientTint;

		vec3 lightDirection = normalize(u_LightPositions[i] - Input.Position);
		float diffuseImpact = max(dot(norm, lightDirection), 0.0);
		diffuseTint += (lightColor * u_LightDiffuseStrengths[i]) * (diffuseImpact * Input.DiffuseTint);

		vec3 viewDirection       = normalize(u_CameraPosition - Input.Position);
		vec3 reflectionDirection = reflect(-lightDirection, norm);
		float specularFactor     = pow(max(dot(viewDirection, reflectionDirection), 0.0), Input.Shininess * 128);
		specularTint += (lightColor * u_LightSpecularStrengths[i]) * (specularFactor * Input.SpecularTint);
	}

	return vec4(ambientTint + diffuseTint + specularTint, 1.0);
}

vec4 GetTextureColor()
{
	vec4 texColor = Input.Color;

	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], Input.TexCoord * Input.TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], Input.TexCoord * Input.TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], Input.TexCoord * Input.TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], Input.TexCoord * Input.TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], Input.TexCoord * Input.TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], Input.TexCoord * Input.TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], Input.TexCoord * Input.TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], Input.TexCoord * Input.TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], Input.TexCoord * Input.TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], Input.TexCoord * Input.TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], Input.TexCoord * Input.TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], Input.TexCoord * Input.TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], Input.TexCoord * Input.TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], Input.TexCoord * Input.TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], Input.TexCoord * Input.TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], Input.TexCoord * Input.TilingFactor); break;
		case 16: texColor *= texture(u_Textures[16], Input.TexCoord * Input.TilingFactor); break;
		case 17: texColor *= texture(u_Textures[17], Input.TexCoord * Input.TilingFactor); break;
		case 18: texColor *= texture(u_Textures[18], Input.TexCoord * Input.TilingFactor); break;
		case 19: texColor *= texture(u_Textures[19], Input.TexCoord * Input.TilingFactor); break;
		case 20: texColor *= texture(u_Textures[20], Input.TexCoord * Input.TilingFactor); break;
		case 21: texColor *= texture(u_Textures[21], Input.TexCoord * Input.TilingFactor); break;
		case 22: texColor *= texture(u_Textures[22], Input.TexCoord * Input.TilingFactor); break;
		case 23: texColor *= texture(u_Textures[23], Input.TexCoord * Input.TilingFactor); break;
		case 24: texColor *= texture(u_Textures[24], Input.TexCoord * Input.TilingFactor); break;
		case 25: texColor *= texture(u_Textures[25], Input.TexCoord * Input.TilingFactor); break;
		case 26: texColor *= texture(u_Textures[26], Input.TexCoord * Input.TilingFactor); break;
		case 27: texColor *= texture(u_Textures[27], Input.TexCoord * Input.TilingFactor); break;
		case 28: texColor *= texture(u_Textures[28], Input.TexCoord * Input.TilingFactor); break;
		case 29: texColor *= texture(u_Textures[29], Input.TexCoord * Input.TilingFactor); break;
		case 30: texColor *= texture(u_Textures[30], Input.TexCoord * Input.TilingFactor); break;
		case 31: texColor *= texture(u_Textures[31], Input.TexCoord * Input.TilingFactor); break;
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
	//o_color = GetTextureColor() * vec4((u_AmbientLightColor * u_AmbientLightIntensity), 1.0);

	// Entity ID buffer
	o_entityID = v_EntityID;
}