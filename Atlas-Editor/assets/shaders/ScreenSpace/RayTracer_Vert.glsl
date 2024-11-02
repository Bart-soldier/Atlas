//--------------------------
// - Atlas Screen-Space Ray-Tracer -
// Renderer Vertex Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in vec4 a_VertexData; // XY: Position, ZW: Texcoords

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec3 u_CameraPosition;
};

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 0) out vec3 v_RayDirection;

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	vec2 texCoords = a_VertexData.zw * 2.0 - 1.0; // (-1 -> 1)

	vec4 target = inverse(u_Projection) * vec4(texCoords.x, texCoords.y, 1.0, 1.0);
	v_RayDirection = vec3(inverse(u_View) * vec4(normalize(vec3(target) / target.w), 0.0));

	gl_Position = vec4(a_VertexData.x, a_VertexData.y, 0.0, 1.0);
}