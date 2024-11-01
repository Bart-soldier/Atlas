//--------------------------
// - Atlas 2D -
// Renderer Circle Fragment Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in      vec3  v_Position;
layout (location = 1) in      vec4  v_Color;
layout (location = 2) in      float v_Thickness;
layout (location = 3) in      float v_Fade;
layout (location = 4) in flat int   v_EntityID;

layout (std140, binding = 0) uniform Settings
{
	float u_Gamma;
};

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
    float distance = 1.0 - length(v_Position);
    float circle   = smoothstep(0.0, v_Fade, distance);
    circle        *= smoothstep(v_Thickness + v_Fade, v_Thickness, distance);

	if (circle == 0.0)
	{
		discard;
	}

    o_Color            = vec4(pow(v_Color.rgb, vec3(u_Gamma)), v_Color.a);
	o_Color.a         *= circle;
	o_EntityID         = v_EntityID;
}