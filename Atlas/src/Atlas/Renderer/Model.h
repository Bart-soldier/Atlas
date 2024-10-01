#pragma once

#include <filesystem>
#include <assimp/scene.h>

#include "Atlas/Renderer/Mesh.h"

namespace Atlas
{
	class Model
	{
	public:
		static std::vector<Mesh> LoadModel(const std::filesystem::path& path);

	private:
		static void ProcessNode(const aiNode& node, const aiScene& scene);
		static Mesh ProcessMesh(const aiMesh& mesh, const aiScene& scene);
	};
}