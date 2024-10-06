#pragma once

#include <filesystem>
#include <assimp/scene.h>

#include "Atlas/Scene/Entity.h"

#include "Atlas/Renderer/Mesh.h"
#include "Atlas/Renderer/Texture.h"

namespace Atlas
{
	class Model
	{
	public:
		static void LoadModel(Ref<Scene> activeScene, const std::filesystem::path& path);

	private:
		static void ProcessNode(Ref<Scene> activeScene, const aiNode& node, const aiScene& modelScene, Entity* parent = nullptr);
		static Ref<Mesh> CreateMesh(const aiMesh& mesh, const aiScene& modelScene);
	};
}