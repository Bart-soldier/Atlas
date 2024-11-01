#include "atlaspch.h"
#include "Atlas/Renderer/ScreenSpaceRenderer.h"

#include <random>

#include "Atlas/Renderer/VertexArray.h"
#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/UniformBuffer.h"
#include "Atlas/Renderer/Shader.h"
#include "Atlas/Renderer/RenderCommand.h"

namespace Atlas
{
	namespace Utils
	{
		static float lerp(float a, float b, float f)
		{
			return a + f * (b - a);
		}
	}

	struct RenderVertex
	{
		glm::vec2 Position;
		glm::vec2 TexCoord;
	};

	struct PostProcessorData
	{
		static const uint32_t RenderVertices = 4;
		static const uint32_t RenderIndices = 6;
		Ref<VertexArray> RenderVertexArray;

		// SSAO
		std::vector<glm::vec3> SSAOSamples;
		Ref<Texture2D> SSAONoise;
		uint32_t SSAOSampleSize;
		Ref<UniformBuffer> SSAOSamplesUniformBuffer;

		// Post Processing Settings
		ScreenSpaceRenderer::Settings SettingsBuffer;
		Ref<UniformBuffer> SettingsUniformBuffer;

		// IBL
		Ref<Texture2D> BRDFLUT;

		// User-related
		Ref<Shader> InversionShader;
		Ref<Shader> GreyscaleShader;
		Ref<Shader> SharpenShader;
		Ref<Shader> BlurShader;
		Ref<Shader> EdgeDetectionShader;
		// Renderer-related
		Ref<Shader> GammaCorrectionShader;
		Ref<Shader> ToneMappingShader;
		Ref<Shader> GaussianBlurShader;
		Ref<Shader> AdditiveBlendingShader;
		Ref<Shader> DeferredLightingShader;
		Ref<Shader> SSAOShader;
		Ref<Shader> SSAOBlurShader;
	};

	static PostProcessorData s_ScreenSpaceRendererData;

	void ScreenSpaceRenderer::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		// Render VAO
		s_ScreenSpaceRendererData.RenderVertexArray = VertexArray::Create();

		// Render VBO
		RenderVertex vertices[s_ScreenSpaceRendererData.RenderVertices];
		vertices[0].Position = { -1.0f, -1.0f };
		vertices[1].Position = { 1.0f, -1.0f };
		vertices[2].Position = { 1.0f,  1.0f };
		vertices[3].Position = { -1.0f,  1.0f };

		vertices[0].TexCoord = { 0.0f,  0.0f };
		vertices[1].TexCoord = { 1.0f,  0.0f };
		vertices[2].TexCoord = { 1.0f,  1.0f };
		vertices[3].TexCoord = { 0.0f,  1.0f };

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, s_ScreenSpaceRendererData.RenderVertices * sizeof(RenderVertex));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_VertexData" } // xy is Position and zw is Texcoords
			});
		s_ScreenSpaceRendererData.RenderVertexArray->AddVertexBuffer(vertexBuffer);

		// Render IBO / EBO
		uint32_t indices[s_ScreenSpaceRendererData.RenderIndices];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, s_ScreenSpaceRendererData.RenderIndices * sizeof(uint32_t));
		s_ScreenSpaceRendererData.RenderVertexArray->SetIndexBuffer(indexBuffer);

		// Uniform Buffers
		s_ScreenSpaceRendererData.SettingsUniformBuffer = UniformBuffer::Create(sizeof(Settings), 3);
		s_ScreenSpaceRendererData.SSAOSamplesUniformBuffer = UniformBuffer::Create(sizeof(glm::vec3) * 64, 4);

		GenerateSSAOData();

		// Shaders
		s_ScreenSpaceRendererData.InversionShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Inversion.glsl");
		s_ScreenSpaceRendererData.GreyscaleShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Greyscale.glsl");
		s_ScreenSpaceRendererData.SharpenShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Sharpen.glsl");
		s_ScreenSpaceRendererData.BlurShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Blur.glsl");
		s_ScreenSpaceRendererData.EdgeDetectionShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_EdgeDetection.glsl");
		s_ScreenSpaceRendererData.GammaCorrectionShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_GammaCorrection.glsl");
		s_ScreenSpaceRendererData.ToneMappingShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_ToneMapping.glsl");
		s_ScreenSpaceRendererData.GaussianBlurShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_GaussianBlur.glsl");
		s_ScreenSpaceRendererData.AdditiveBlendingShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_AdditiveTextureBlending.glsl");
		s_ScreenSpaceRendererData.DeferredLightingShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_PBRDeferredLighting.glsl");
		s_ScreenSpaceRendererData.SSAOShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_SSAO.glsl");
		s_ScreenSpaceRendererData.SSAOBlurShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_SSAO_Blur.glsl");

		// IBL
		s_ScreenSpaceRendererData.BRDFLUT = Texture2D::Create("assets/luts/brdf_lut.png");
	}

	void ScreenSpaceRenderer::RenderPostProcessingEffect(const uint32_t& renderID, const PostProcessingEffects& effect, const Settings& settings)
	{
		ATLAS_PROFILE_FUNCTION();

		s_ScreenSpaceRendererData.SettingsBuffer.Strength = settings.Strength;
		s_ScreenSpaceRendererData.SettingsBuffer.KernelOffset = settings.KernelOffset;
		s_ScreenSpaceRendererData.SettingsUniformBuffer->SetData(&s_ScreenSpaceRendererData.SettingsBuffer, sizeof(Settings));

		RenderCommand::BindTextureSlot(0, renderID);

		switch (effect)
		{
		default:
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::None:
			return;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::Inversion:
			s_ScreenSpaceRendererData.InversionShader->Bind();
			break;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::Greyscale:
			s_ScreenSpaceRendererData.GreyscaleShader->Bind();
			break;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::Sharpen:
			s_ScreenSpaceRendererData.SharpenShader->Bind();
			break;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::Blur:
			s_ScreenSpaceRendererData.BlurShader->Bind();
			break;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::EdgeDetection:
			s_ScreenSpaceRendererData.EdgeDetectionShader->Bind();
			break;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::GammaCorrection:
			s_ScreenSpaceRendererData.GammaCorrectionShader->Bind();
			break;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::ToneMapping:
			s_ScreenSpaceRendererData.ToneMappingShader->Bind();
			break;
		case Atlas::ScreenSpaceRenderer::PostProcessingEffects::Bloom:
			s_ScreenSpaceRendererData.GaussianBlurShader->Bind();
			break;
		}

		RenderCommand::DrawIndexed(s_ScreenSpaceRendererData.RenderVertexArray, s_ScreenSpaceRendererData.RenderIndices);
	}

	void ScreenSpaceRenderer::RenderAdditiveTextureBlending(const uint32_t& texture1ID, const uint32_t& texture2ID)
	{
		RenderCommand::BindTextureSlot(0, texture1ID);
		RenderCommand::BindTextureSlot(1, texture2ID);

		s_ScreenSpaceRendererData.AdditiveBlendingShader->Bind();

		RenderCommand::DrawIndexed(s_ScreenSpaceRendererData.RenderVertexArray, s_ScreenSpaceRendererData.RenderIndices);
	}

	void ScreenSpaceRenderer::RenderDeferredShading(const uint32_t& positionTexID, const uint32_t& normalTexID, const uint32_t& albedoTexID, const uint32_t& materialTexID,
		const uint32_t& ssaoTexID, const Ref<Cubemap>& skybox)
	{
		RenderCommand::BindTextureSlot(0, positionTexID);
		RenderCommand::BindTextureSlot(1, normalTexID);
		RenderCommand::BindTextureSlot(2, albedoTexID);
		RenderCommand::BindTextureSlot(3, materialTexID);
		RenderCommand::BindTextureSlot(4, ssaoTexID);
		s_ScreenSpaceRendererData.BRDFLUT->Bind(5);
		skybox->BindIrradianceMap(6);
		skybox->BindPreFilteredMap(7);

		s_ScreenSpaceRendererData.DeferredLightingShader->Bind();

		RenderCommand::DrawIndexed(s_ScreenSpaceRendererData.RenderVertexArray, s_ScreenSpaceRendererData.RenderIndices);
	}

	void ScreenSpaceRenderer::RenderSSAO(const uint32_t& positionTexID, const uint32_t& normalTexID)
	{
		s_ScreenSpaceRendererData.SettingsBuffer.Strength = s_ScreenSpaceRendererData.SSAOSampleSize;
		s_ScreenSpaceRendererData.SettingsBuffer.KernelOffset = 0.5f;
		s_ScreenSpaceRendererData.SettingsUniformBuffer->SetData(&s_ScreenSpaceRendererData.SettingsBuffer, sizeof(Settings));

		RenderCommand::BindTextureSlot(0, positionTexID);
		RenderCommand::BindTextureSlot(1, normalTexID);

		s_ScreenSpaceRendererData.SSAOShader->Bind();

		RenderCommand::DrawIndexed(s_ScreenSpaceRendererData.RenderVertexArray, s_ScreenSpaceRendererData.RenderIndices);
	}

	void ScreenSpaceRenderer::RenderSSAOBlur(const uint32_t& ssaoTexID)
	{
		RenderCommand::BindTextureSlot(0, ssaoTexID);

		s_ScreenSpaceRendererData.SSAOBlurShader->Bind();

		RenderCommand::DrawIndexed(s_ScreenSpaceRendererData.RenderVertexArray, s_ScreenSpaceRendererData.RenderIndices);
	}

	void ScreenSpaceRenderer::GenerateSSAOData()
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;
		s_ScreenSpaceRendererData.SSAOSampleSize = 64;

		s_ScreenSpaceRendererData.SSAOSamples.reserve(s_ScreenSpaceRendererData.SSAOSampleSize);

		for (unsigned int i = 0; i < s_ScreenSpaceRendererData.SSAOSampleSize; ++i)
		{
			glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);

			sample = glm::normalize(sample);
			sample *= randomFloats(generator);

			float scale = (float)i / (float)s_ScreenSpaceRendererData.SSAOSampleSize;
			scale = Utils::lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;

			s_ScreenSpaceRendererData.SSAOSamples.push_back(sample);
		}

		std::vector<glm::vec3> ssaoNoise;
		ssaoNoise.reserve(16);

		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
			ssaoNoise.push_back(noise);
		}

		TextureSpecification noiseTextureSpecification = TextureSpecification();
		noiseTextureSpecification.Width = 4;
		noiseTextureSpecification.Height = 4;
		noiseTextureSpecification.Format = ImageFormat::RGB16F;
		noiseTextureSpecification.GenerateMips = false;
		noiseTextureSpecification.MinFilter = ResizeFilter::Nearest;
		noiseTextureSpecification.MagFilter = ResizeFilter::Nearest;
		noiseTextureSpecification.WrapS = Wrap::ClampToEdge;
		noiseTextureSpecification.WrapT = Wrap::ClampToEdge;

		s_ScreenSpaceRendererData.SSAONoise = Texture2D::Create(noiseTextureSpecification);
		s_ScreenSpaceRendererData.SSAONoise->SetData(ssaoNoise.data(), ssaoNoise.size() * sizeof(glm::vec3));

		s_ScreenSpaceRendererData.SSAOSamplesUniformBuffer->SetData(s_ScreenSpaceRendererData.SSAOSamples.data(), sizeof(glm::vec3) * s_ScreenSpaceRendererData.SSAOSampleSize);
	}
}