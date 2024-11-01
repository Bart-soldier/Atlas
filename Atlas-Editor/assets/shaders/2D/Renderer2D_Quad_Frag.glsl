//--------------------------
// - Atlas 2D -
// Renderer Quad Fragment Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in      vec4  v_Color;
layout (location = 1) in      vec2  v_TexCoord;
layout (location = 2) in      float v_TilingFactor;
layout (location = 3) in flat uint  v_TexIndex;
layout (location = 4) in flat int   v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

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
	vec4 texColor = v_Color * texture(u_Textures[v_TexIndex], v_TexCoord * v_TilingFactor);

	if (texColor.a == 0.0)
	{
		discard;
	}

	o_Color            = vec4(pow(texColor.rgb, vec3(u_Gamma)), texColor.a);
	o_EntityID         = v_EntityID;
}