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
			Greyscale,
			Numerical,
			Blur,
			EdgeDetection,
			GammaCorrection
		};

		static void Init();

		static void ApplyPostProcessingEffect(const uint32_t& renderID, const PostProcessingEffect& effect, const float& kernelOffset);
	};
}