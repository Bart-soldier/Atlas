//--------------------------
// - Atlas 3D -
// Renderer Outline Fragment Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in      vec4 v_Color;
layout (location = 1) in flat int  v_EntityID;

layout (std140, binding = 0) uniform Settings
{
	float u_Gamma;
};

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;

void main()
{
	if (v_Color.a == 0.0)
	{
		discard;
	}

	o_Color            = vec4(pow(v_Color.rgb, vec3(u_Gamma)), v_Color.a);
	o_EntityID         = v_EntityID;
}