//--------------------------
// - Atlas 3D -
// Renderer3D Outline Shader
// --------------------------

#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in int  a_EntityID;

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};

layout (location = 0) out      vec4 v_Color;
layout (location = 1) out flat int  v_EntityID;

void main()
{
	v_Color    = a_Color;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int  o_EntityID;
layout (location = 2) out vec4 o_PostProcessColor;

layout (location = 0) in      vec4 v_Color;
layout (location = 1) in flat int  v_EntityID;

layout (std140, binding = 0) uniform Settings
{
	float u_Gamma;
};

void main()
{
	if (v_Color.a == 0.0)
	{
		discard;
	}

	o_Color            = vec4(pow(v_Color.rgb, vec3(u_Gamma)), v_Color.a);
	o_EntityID         = v_EntityID;
	o_PostProcessColor = o_Color;
}