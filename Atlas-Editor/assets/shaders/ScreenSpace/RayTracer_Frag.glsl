//--------------------------
// - Atlas Screen-Space Ray-Tracer -
// Renderer Fragment Shader
// --------------------------

#version 450 core

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

layout (std140, binding = 3) uniform Settings
{
	float u_Strength;
	float u_KernelOffset;
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
	vec2 texCoords = v_TexCoords * 2.0 - 1.0; // (-1 -> 1)

	vec3 rayOrigin    = u_CameraPosition.xyz;
	vec3 rayDirection = vec3(texCoords, -1.0); // z depends on coordinate system
	//rayDirection = normalize(rayDirection);
	float radius = 0.5;

	//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z;
	float a = dot(rayDirection, rayDirection);
	float b = 2.0 * (dot(rayOrigin, rayDirection));
	float c = dot(rayOrigin, rayOrigin) - radius * radius;

	float discriminant = b * b - 4.0 * a * c;

	o_Color = discriminant >= 0.0 ? vec4(1.0, 0.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
}