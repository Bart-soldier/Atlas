//--------------------------
// - Atlas Post-Processing -
// Vertex Shader
// --------------------------

#version 450 core

layout (location = 0) in vec4 a_VertexData; // xy is Position and zw is Texcoords

layout (location = 0) out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_VertexData.zw;

	gl_Position = vec4(a_VertexData.x, a_VertexData.y, 0.0, 1.0);
}