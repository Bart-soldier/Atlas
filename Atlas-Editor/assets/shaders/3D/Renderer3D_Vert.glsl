//--------------------------
// - Atlas 3D -
// Renderer 3D Vertex Shader
// --------------------------

#version 450 core

layout(location = 0)  in vec4 a_Position_ID;                        // XYZ: Position, W: EntityID (editor-only)
layout(location = 1)  in vec3 a_Normal;
layout(location = 2)  in vec2 a_TexCoord;
layout(location = 3)  in vec3 a_Tangent;
layout(location = 4)  in vec3 a_Bitangent;

layout(location = 5)  in mat4 a_Model;

layout(location = 9)  in vec3 a_Color;
layout(location = 10)  in vec2 a_Metallic_Roughness;                 // X: Metallic,  Y: Roughness

layout(location = 11) in vec3 a_Albedo_Normal_Metallic_TexIndex;    // X: Albedo,    Y: Normal, Z: Metallic
layout(location = 12) in vec3 a_Roughness_AO_Displacement_TexIndex; // X: Roughness, Y: AO,     Z: Displacement

layout (binding = 0) uniform sampler2D u_Textures[32];

layout (std140, binding = 1) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Projection;
	mat4 u_View;
	vec4 u_CameraPosition;
};

//struct VertexData
//{
//	vec3  Position;
//	vec3  Normal;
//	vec2  TexCoord;
//	mat3  TBN;
//
//	vec3  Color;
//	float Metallic;
//	float Roughness;
//};

layout (location = 0)   out flat int   v_EntityID;
layout (location = 1)   out      vec3  v_Position;
layout (location = 2)   out      vec3  v_Normal;
layout (location = 3)   out      vec2  v_TexCoord;
layout (location = 4)   out      vec3  v_Color;
layout (location = 5)   out      float v_Metallic;
layout (location = 6)   out      float v_Roughness;
layout (location = 7)   out      mat3  v_TBN;
layout (location = 10)  out flat vec3  v_Albedo_Normal_Metallic_TexIndex;
layout (location = 11)  out flat vec3  v_Roughness_AO_Displacement_TexIndex;

void main()
{
	v_EntityID                           = int(a_Position_ID.w);

	v_Position                           = a_Position_ID.xyz;
	v_Normal                             = a_Normal;
	v_TexCoord                           = a_TexCoord;

	v_Color                              = a_Color;
	v_Metallic                           = a_Metallic_Roughness.x;
	v_Roughness                          = a_Metallic_Roughness.y;

	v_Albedo_Normal_Metallic_TexIndex    = a_Albedo_Normal_Metallic_TexIndex;
	v_Roughness_AO_Displacement_TexIndex = a_Roughness_AO_Displacement_TexIndex;

//	vec3 T = normalize(a_Tangent);
//	vec3 N = normalize(a_Normal);
//	// Re-orthogonalize T with respect to N (Gram-Schmidt)
//	T = normalize(T - dot(T, N) * N);
//	vec3 B = cross(N, T);
//	if (dot(cross(N, T), B) < 0.0)
//	{
//		T = T * -1.0;
//	}

	vec3 T = normalize(a_Tangent);
	vec3 B = normalize(a_Bitangent);
	vec3 N = normalize(a_Normal);
	// Re-orthogonalize T with respect to N (Gram-Schmidt)
	T = normalize(T - dot(T, N) * N);
	// Re-orthogonalize B with respect to N (Gram-Schmidt)
	B = normalize(B - dot(B, N) * N);
//	if (dot(cross(N, T), B) < 0.0)
//	{
//		T = T * -1.0;
//	}

//	   vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
//   vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
//   vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
//   mat3 TBN = mat3(T, B, N);

	v_TBN = mat3(T, B, N);

	gl_Position = u_ViewProjection * vec4(a_Position_ID.xyz, 1.0);
}