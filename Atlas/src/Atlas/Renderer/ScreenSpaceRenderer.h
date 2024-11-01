#pragma once
#pragma once

#include "Atlas/Renderer/Texture.h"

namespace Atlas
{
	class ScreenSpaceRenderer
	{
	public:
		enum class PostProcessingEffects
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

		static void RenderPostProcessingEffect(const uint32_t& renderID, const PostProcessingEffects& effect, const Settings& settings);
		static void RenderAdditiveTextureBlending(const uint32_t& texture1ID, const uint32_t& texture2ID);
		static void RenderDeferredShading(const uint32_t& positionTexID, const uint32_t& normalTexID, const uint32_t& albedoTexID, const uint32_t& materialTexID,
			const uint32_t& ssaoTexID);
		static void RenderSSAO(const uint32_t& positionTexID, const uint32_t& normalTexID);
		static void RenderSSAOBlur(const uint32_t& ssaoTexID);

	private:
		static void GenerateSSAOData();
	};
}