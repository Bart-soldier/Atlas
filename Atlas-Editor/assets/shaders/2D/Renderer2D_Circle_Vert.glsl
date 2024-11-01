//--------------------------
// - Atlas 2D -
// Renderer Circle Vertex Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in vec3  a_WorldPosition;
layout (location = 1) in vec3  a_LocalPosition;
layout (location = 2) in vec4  a_Color;
layout (location = 3) in float a_Thickness;
layout (location = 4) in float a_Fade;
layout (location = 5) in int  a_EntityID;

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec4 u_CameraPosition;
};

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 0) out      vec3  v_Position;
layout (location = 1) out      vec4  v_Color;
layout (location = 2) out      float v_Thickness;
layout (location = 3) out      float v_Fade;
layout (location = 4) out flat int   v_EntityID;

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	v_Position      = a_LocalPosition;
	v_Color         = a_Color;
	v_Thickness     = a_Thickness;
	v_Fade          = a_Fade;

	v_EntityID      = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}