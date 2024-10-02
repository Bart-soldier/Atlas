#pragma once

#include <filesystem>
#include <assimp/scene.h>

#include "Atlas/Renderer/Mesh.h"
#include "Atlas/Renderer/Texture.h"

namespace Atlas
{
	class Model
	{
	public:
		struct ModelData
		{
			std::vector<Ref<Mesh>> Meshes;
			//std::vector<Ref<Texture2D>>
		};

		static const ModelData& LoadModel(const std::filesystem::path& path);

	private:
		static void ProcessNode(const aiNode& node, const aiScene& scene);
		static Ref<Mesh> ProcessMesh(const aiMesh& mesh, const aiScene& scene);
	};
}