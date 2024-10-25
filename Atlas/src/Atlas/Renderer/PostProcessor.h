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
			Sharpen,
			Blur,
			EdgeDetection,
			GammaCorrection,
			ToneMapping
		};

		struct Settings
		{
			float Strength = 1.0f;
			float KernelOffset = 300.0f;

			Settings(float strength = 1.0f, float kernelOffset = 300.0f)
				: Strength(strength), KernelOffset(kernelOffset) {}
		};

		static void Init();

		static void ApplyPostProcessingEffect(const uint32_t& renderID, const PostProcessingEffect& effect, const Settings& settings);
	};
}