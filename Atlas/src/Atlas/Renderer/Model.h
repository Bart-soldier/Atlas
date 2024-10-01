#pragma once

#include <filesystem>
#include <assimp/scene.h>

namespace Atlas
{
	class Model
	{
	public:
		static void LoadModel(const std::filesystem::path& path);

	private:
		static void ProcessNode(const aiNode& node, const aiScene& scene);
	};
}