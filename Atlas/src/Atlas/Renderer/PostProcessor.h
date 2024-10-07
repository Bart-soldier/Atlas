#pragma once

#include "Atlas/Renderer/Texture.h"

namespace Atlas
{
	class PostProcessor
	{
	public:
		enum class PostProcessingEffect
		{
			None = 0,
			Inversion,
			Greyscale
		};

		static void Init();

		static void ApplyPostProcessingEffect(PostProcessingEffect effect, uint32_t renderID);
	};
}