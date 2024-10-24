#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Mesh
	{
	public:
		enum class MeshPresets
		{
			Custom = 0,
			Square = 1,
		};

		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 TexCoords;
			glm::vec3 Tangent;
			glm::vec3 Bitangent;

			Vertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float texU, float texV,
				float tanX = 1.0f, float tanY = 0.0f, float tanZ = 0.0f, float bitanX = 0.0f, float bitanY = 1.0f, float bitanZ = 0.0f)
				: Position({ posX, posY, posZ }), Normal({ normX, normY, normZ }), TexCoords({ texU, texV }),
				  Tangent({tanX, tanY, tanZ}), Bitangent({ bitanX, bitanY, bitanZ })
			{}
			Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords,
				glm::vec3 tangent = { 1.0f, 0.0f, 0.0f }, glm::vec3 bitangent = { 0.0f, 1.0f, 0.0f })
				: Position(position), Normal(normal), TexCoords(texCoords),
				Tangent(tangent), Bitangent(bitangent)
			{}
		};

		Mesh() { SetMeshPreset(MeshPresets::Square); }
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
			: m_Vertices(vertices), m_Indices(indices) { SetMeshPreset(MeshPresets::Custom); }

		void SetVertices(const std::vector<Vertex>& vertices) { m_Vertices = vertices; }
		const std::vector<Vertex>& GetVertices() { return m_Vertices; }
		void SetIndices(const std::vector<uint32_t>& indices) { m_Indices = indices; }
		const std::vector<uint32_t>& GetIndices() { return m_Indices; }

		void SetMeshPreset(const MeshPresets& materialPreset);
		const MeshPresets& GetMeshPreset() { return m_MeshPreset; }

	private:
		void CalculateSquareVertices();
		void CalculateTangents();

		MeshPresets m_MeshPreset;

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}