//--------------------------
// - Atlas Screen-Space Ray-Tracer -
// Renderer Fragment Shader
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

layout (binding = 0) uniform sampler2D u_screenTexture;

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

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	int width = 1816;
	int height = 994;
	float aspectRatio = width / float(height);

	vec2 texCoords = v_TexCoords * 2.0 - 1.0; // (-1 -> 1)
	texCoords.x *= aspectRatio;

	vec3 sphereOrigin = vec3(0.0);
	float sphereRadius = 0.5;

	//vec3 lightDirection = u_Lights[0].Direction.xyz;
	vec3 lightDirection = vec3(-1.0, -1.0, 1.0);
	lightDirection = normalize(lightDirection);

	vec3 rayOrigin    = u_CameraPosition.xyz;
	vec3 rayDirection = vec3(texCoords, -1.0); // z depends on coordinate system
	//rayDirection = normalize(rayDirection);

	//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z;
	float a = dot(rayDirection, rayDirection);
	float b = 2.0 * (dot(rayOrigin, rayDirection));
	float c = dot(rayOrigin, rayOrigin) - sphereRadius * sphereRadius;

	float discriminant = b * b - 4.0 * a * c;

	vec4 fragColor = vec4(0.0, 0.0, 0.0, 1.0);

	if(discriminant >= 0)
	{
		float t0 = (-b - sqrt(discriminant)) / (2.0 * a);
		float t1 = (-b + sqrt(discriminant)) / (2.0 * a);
		
		vec3 hitPosition0 = rayOrigin + rayDirection * t0;
		vec3 hitPosition1 = rayOrigin + rayDirection * t1;

		vec3 closestHit = min(hitPosition0, hitPosition1);

		vec3 normal = closestHit - sphereOrigin;
		normal = normalize(normal);

		float lightIntensity = max(dot(normal, -lightDirection), 0.0);

		fragColor = vec4(lightIntensity * vec3(1.0, 0.0, 1.0), 1.0);
	}

	o_Color = fragColor;
}