#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 TexCoords;

			Vertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float texU, float texV)
				: Position(glm::vec3(posX, posY, posZ)), Normal(glm::vec3(normX, normY, normZ)), TexCoords(glm::vec2(texU, texV)) {}
			Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords)
				: Position(position), Normal(normal), TexCoords(texCoords) {}
		};

		Mesh() = default;
		Mesh(const Mesh&) = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
			: m_Vertices(vertices), m_Indices(indices) {}

		void SetVertices(const std::vector<Vertex>& vertices) { m_Vertices = vertices; }
		const std::vector<Vertex>& GetVertices() { return m_Vertices; }
		void SetIndices(const std::vector<uint32_t>& indices) { m_Indices = indices; }
		const std::vector<uint32_t>& GetIndices() { return m_Indices; }

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}