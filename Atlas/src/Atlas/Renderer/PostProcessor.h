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
			// User-related
			Inversion,
			Greyscale,
			Sharpen,
			Blur,
			EdgeDetection,

			// Renderer-related
			GammaCorrection,
			ToneMapping,
			Bloom,
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
		static void ApplyAdditiveTextureBlending(const uint32_t& texture1ID, const uint32_t& texture2ID);
	};
}