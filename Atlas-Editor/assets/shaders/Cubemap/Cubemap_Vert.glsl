//--------------------------
// - Atlas Cubemap -
// Vertex Shader
// --------------------------

#version 450 core

layout (location = 0) in vec3 a_Position;

layout (std140, binding = 4) uniform ViewProjection
{
	mat4 u_Projection;
	mat4 u_View;
};

layout (location = 0) out vec3 v_TexCoords;

void main()
{
	v_TexCoords = a_Position;

	gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
}