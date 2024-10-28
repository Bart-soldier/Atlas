//--------------------------
// - Atlas Post-Processing -
// SSAO Fragment Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 v_TexCoords;

// 0: Position
// 1: Normal
// 2: SSAO Noise
layout (binding = 0) uniform sampler2D u_ScreenTextures[3];

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec4 u_CameraPosition;
};

layout (std140, binding = 3) uniform Settings
{
	float u_KernelSize;
	float u_Radius;
};

layout (std140, binding = 4) uniform SSAOSamples
{
	vec3 u_Samples[64];
};

layout (location = 0) out vec4 o_Color;

const float g_Bias = 0.025;

void main()
{
	vec2 screenSize = textureSize(u_ScreenTextures[0], 0);
	vec2 noiseSize  = textureSize(u_ScreenTextures[2], 0);
    const vec2 noiseScale = vec2(screenSize.x/noiseSize.x, screenSize.y/noiseSize.y);

	vec4 position  = u_View * texture(u_ScreenTextures[0], v_TexCoords);
	vec3 normal    = mat3(u_View) * texture(u_ScreenTextures[1], v_TexCoords).rgb;
	vec3 randomVec = texture(u_ScreenTextures[2], v_TexCoords * noiseScale).xyz;

	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);
	float occlusion = 0.0;

	for(int i = 0; i < u_KernelSize; ++i)
	{
		vec4 samplePos = position + vec4(TBN * u_Samples[i] * u_Radius, 1.0);
    
		vec4 offset = samplePos;
		offset      = u_Projection * offset;
		offset.xyz /= offset.w;
		offset.xyz  = offset.xyz * 0.5 + 0.5;

		float sampleDepth = vec3(u_View * texture(u_ScreenTextures[0], offset.xy)).z;

		float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(position.z - sampleDepth));
		occlusion       += (sampleDepth >= samplePos.z + g_Bias ? 1.0 : 0.0) * rangeCheck; 
	}

	occlusion = 1.0 - (occlusion / u_KernelSize);

	o_Color = vec4(vec3(occlusion), 1.0);
}