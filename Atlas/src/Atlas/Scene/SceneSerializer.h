#pragma once

#include "Atlas/Scene/Scene.h"

namespace Atlas
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::filesystem::path& path);
		void SerializeRuntime(const std::filesystem::path& path);

		bool Deserialize(const std::filesystem::path& path);
		bool DeserializeRuntime(const std::filesystem::path& path);

	private:
		Ref<Scene> m_Scene;
	};
}


