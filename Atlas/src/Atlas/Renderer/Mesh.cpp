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
				m_Vertices.clear();
				m_Vertices.reserve(36);
				m_Indices.clear();
				m_Indices.reserve(36);

				m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f });

				m_Vertices.push_back({ -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f });

				m_Vertices.push_back({ -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f });
				m_Vertices.push_back({ -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f });

				m_Vertices.push_back({ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f });

				m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f });
				m_Vertices.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f });

				m_Vertices.push_back({ -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f });
				m_Vertices.push_back({ -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f });
				m_Vertices.push_back({ -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0 });

				for (int i = 0; i < 36; i++)
				{
					m_Indices.push_back(i);
				}

				break;
		}
	}
}