//--------------------------
// - Atlas 3D -
// Renderer 3D PBR Deferred Lighting Fragment Shader
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
// 3: X: Metallic, Y: Roughness, Z: AO
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

vec4 CalculateBrightColor(vec3 fragmentColor);

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	vec3  position         = texture(u_ScreenTextures[0], v_TexCoords).rgb;
	vec3  normal           = texture(u_ScreenTextures[1], v_TexCoords).rgb;
	vec3  albedo           = texture(u_ScreenTextures[2], v_TexCoords).rgb;
	float metallic         = texture(u_ScreenTextures[3], v_TexCoords).x;
	float roughness        = texture(u_ScreenTextures[3], v_TexCoords).y;
	float ambientOcclusion = texture(u_ScreenTextures[4], v_TexCoords).z;

	vec3 viewDirection  = normalize(u_CameraPosition.xyz - position);

	vec3 fragmentColor = vec3(1.0);

	o_Color = vec4(fragmentColor.rgb, 1.0);
	o_BrightColor = CalculateBrightColor(fragmentColor);
}

/* ------------------------------ */
/* --- METHOD IMPLEMENTATIONS --- */
/* ------------------------------ */

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
