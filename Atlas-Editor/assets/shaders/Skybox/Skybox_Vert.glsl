//--------------------------
// - Atlas Skybox -
// Vertex Shader
// --------------------------

#version 450 core

layout (location = 0) in vec3 a_Position;

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec4 u_CameraPosition;
};

layout (location = 0) out vec3 v_TexCoords;

void main()
{
	v_TexCoords = a_Position;

	vec4 position = u_Projection * mat4(mat3(u_View)) * vec4(a_Position, 1.0);
	gl_Position = position.xyww;
}