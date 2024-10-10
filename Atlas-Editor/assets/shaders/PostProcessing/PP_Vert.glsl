//--------------------------
// - Atlas Post-Processing -
// Vertex Shader
// --------------------------

#version 450 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoords;

layout (location = 0) out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;

	gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}