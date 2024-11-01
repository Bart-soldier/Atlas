//--------------------------
// - Atlas Screen-Space Renderer -
// Renderer Vertex Shader
// --------------------------

#version 450 core

/* ------------------------------ */
/* ----------- INPUTS ----------- */
/* ------------------------------ */

layout (location = 0) in vec4 a_VertexData; // XY: Position, ZW: Texcoords

/* ------------------------------ */
/* ----------- OUTPUTS ---------- */
/* ------------------------------ */

layout (location = 0) out vec2 v_TexCoords;

/* ------------------------------ */
/* ------------ MAIN ------------ */
/* ------------------------------ */

void main()
{
	v_TexCoords = a_VertexData.zw;

	gl_Position = vec4(a_VertexData.x, a_VertexData.y, 0.0, 1.0);
}