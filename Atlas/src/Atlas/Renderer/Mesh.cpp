#include "atlaspch.h"
#include "Atlas/Renderer/Mesh.h"

namespace Atlas
{
	void Mesh::SetMeshPreset(const MeshPresets& meshPresets)
	{
		if (m_MeshPreset == meshPresets)
		{
			return;
		}

		m_MeshPreset = meshPresets;

		switch (m_MeshPreset)
		{
			default:
			case Atlas::Mesh::MeshPresets::Custom:
				return;
			case Atlas::Mesh::MeshPresets::Square:
				CalculateSquareVertices();
				CalculateTangents();
				break;
		}
	}
	void Mesh::CalculateSquareVertices()
	{
		// All indices are in counter-clockwise order (for culling)
		m_Vertices.clear();
		m_Vertices.reserve(24); // 4 vertices per face, 6 faces
		m_Indices.clear();
		m_Indices.reserve(36); // 3 indices per triangle, 2 triangles per face, 6 faces

		// Back face
		m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f }); // 0: Bottom-left
		m_Vertices.push_back({  0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f }); // 1: Top-right
		m_Vertices.push_back({  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f }); // 2: Bottom-right
		m_Vertices.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f }); // 3: Top-left
		m_Indices.push_back(0);
		m_Indices.push_back(1);
		m_Indices.push_back(2);
		m_Indices.push_back(3);
		m_Indices.push_back(1);
		m_Indices.push_back(0);

		// Front face
		m_Vertices.push_back({ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f }); // 4: Bottom-left
		m_Vertices.push_back({  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f }); // 5: Top-right
		m_Vertices.push_back({  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f }); // 6: Bottom-right
		m_Vertices.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }); // 7: Top-left
		m_Indices.push_back(4);
		m_Indices.push_back(6);
		m_Indices.push_back(5);
		m_Indices.push_back(7);
		m_Indices.push_back(4);
		m_Indices.push_back(5);

		// Left face
		m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f }); // 8: Bottom-left
		m_Vertices.push_back({ -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f }); // 9: Top-right
		m_Vertices.push_back({ -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f }); // 10: Bottom-right
		m_Vertices.push_back({ -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f }); // 11: Top-left
		m_Indices.push_back(8);
		m_Indices.push_back(10);
		m_Indices.push_back(9);
		m_Indices.push_back(11);
		m_Indices.push_back(8);
		m_Indices.push_back(9);

		// Right face
		m_Vertices.push_back({ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f }); // 12: Bottom-left
		m_Vertices.push_back({ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f }); // 13: Top-right
		m_Vertices.push_back({ 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f }); // 14: Bottom-right
		m_Vertices.push_back({ 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f }); // 15: Top-left
		m_Indices.push_back(12);
		m_Indices.push_back(13);
		m_Indices.push_back(14);
		m_Indices.push_back(15);
		m_Indices.push_back(13);
		m_Indices.push_back(12);

		// Bottom face
		m_Vertices.push_back({  0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f }); // 16: Bottom-left
		m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f }); // 17: Top-right
		m_Vertices.push_back({ -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f }); // 18: Bottom-right
		m_Vertices.push_back({  0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f }); // 19: Top-left
		m_Indices.push_back(16);
		m_Indices.push_back(18);
		m_Indices.push_back(17);
		m_Indices.push_back(19);
		m_Indices.push_back(16);
		m_Indices.push_back(17);

		// Top face
		m_Vertices.push_back({  0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f }); // 20: Bottom-left
		m_Vertices.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f }); // 21: Top-right
		m_Vertices.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f }); // 22: Bottom-right
		m_Vertices.push_back({  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f }); // 23: Top-left
		m_Indices.push_back(20);
		m_Indices.push_back(21);
		m_Indices.push_back(22);
		m_Indices.push_back(23);
		m_Indices.push_back(21);
		m_Indices.push_back(20);
	}

	void Mesh::CalculateTangents()
	{
		for (int i = 0; i < m_Indices.size(); i += 3)
		{
			Vertex& vertex1 = m_Vertices[m_Indices[i    ]];
			Vertex& vertex2 = m_Vertices[m_Indices[i + 1]];
			Vertex& vertex3 = m_Vertices[m_Indices[i + 2]];

			glm::vec3 edge1 = vertex2.Position - vertex1.Position;
			glm::vec3 edge2 = vertex3.Position - vertex1.Position;
			glm::vec2 deltaUV1 = vertex2.TexCoords - vertex1.TexCoords;
			glm::vec2 deltaUV2 = vertex3.TexCoords - vertex1.TexCoords;

			glm::vec3 tangent;
			glm::vec3 bitangent;
			float fraction = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			vertex1.Tangent   = fraction * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
			vertex1.Bitangent = fraction * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);
		}
	}
}