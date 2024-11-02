//--------------------------
// - Atlas 2D -
// Renderer Quad Vertex Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec4  a_Color;
layout (location = 2) in vec2  a_TexCoord;
layout (location = 3) in uint  a_TexIndex;
layout (location = 4) in float a_TilingFactor;
layout (location = 5) in int   a_EntityID;

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

layout (location = 0) out      vec4  v_Color;
layout (location = 1) out      vec2  v_TexCoord;
layout (location = 2) out      float v_TilingFactor;
layout (location = 3) out flat uint  v_TexIndex;
layout (location = 4) out flat int   v_EntityID;

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	v_Color        = a_Color;
	v_TexCoord     = a_TexCoord;
	v_TilingFactor = a_TilingFactor;
	v_TexIndex     = a_TexIndex;

	v_EntityID     = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}