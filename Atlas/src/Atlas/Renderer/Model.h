#pragma once

#include "Atlas/Renderer/Mesh.h"

namespace Atlas
{
	class Model
	{
	public:
		Model(const std::filesystem::path& path);

	private:
		std::filesystem::path m_Path;
		//std::vector
	};
}