#include "atlaspch.h"
#include "Atlas/Renderer/PostProcessor.h"

#include "Atlas/Renderer/VertexArray.h"
#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/UniformBuffer.h"
#include "Atlas/Renderer/Shader.h"
#include "Atlas/Renderer/RenderCommand.h"

namespace Atlas
{
	struct RenderVertex
	{
		glm::vec2 Position;
		glm::vec2 TexCoord;
	};

	struct PostProcessorData
	{
		static const uint32_t RenderVertices = 4;
		static const uint32_t RenderIndices  = 6;
		Ref<VertexArray> RenderVertexArray;

		// Post Processing Settings
		PostProcessor::Settings SettingsBuffer;
		Ref<UniformBuffer> SettingsUniformBuffer;

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
	};

	static PostProcessorData s_PostProcessorData;

	void PostProcessor::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		// Render VAO
		s_PostProcessorData.RenderVertexArray = VertexArray::Create();

		// Render VBO
		RenderVertex vertices[s_PostProcessorData.RenderVertices];
		vertices[0].Position = { -1.0f, -1.0f };
		vertices[1].Position = {  1.0f, -1.0f };
		vertices[2].Position = {  1.0f,  1.0f };
		vertices[3].Position = { -1.0f,  1.0f };

		vertices[0].TexCoord = {  0.0f,  0.0f };
		vertices[1].TexCoord = {  1.0f,  0.0f };
		vertices[2].TexCoord = {  1.0f,  1.0f };
		vertices[3].TexCoord = {  0.0f,  1.0f };

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, s_PostProcessorData.RenderVertices * sizeof(RenderVertex));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_VertexData" } // xy is Position and zw is Texcoords
		});
		s_PostProcessorData.RenderVertexArray->AddVertexBuffer(vertexBuffer);

		// Render IBO / EBO
		uint32_t indices[s_PostProcessorData.RenderIndices];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, s_PostProcessorData.RenderIndices * sizeof(uint32_t));
		s_PostProcessorData.RenderVertexArray->SetIndexBuffer(indexBuffer);

		// Uniform Buffers
		s_PostProcessorData.SettingsUniformBuffer = UniformBuffer::Create(sizeof(Settings), 3);

		// Shaders
		s_PostProcessorData.InversionShader        = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Inversion.glsl"               );
		s_PostProcessorData.GreyscaleShader        = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Greyscale.glsl"               );
		s_PostProcessorData.SharpenShader          = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Sharpen.glsl"                 );
		s_PostProcessorData.BlurShader             = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Blur.glsl"                    );
		s_PostProcessorData.EdgeDetectionShader    = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_EdgeDetection.glsl"           );
		s_PostProcessorData.GammaCorrectionShader  = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_GammaCorrection.glsl"         );
		s_PostProcessorData.ToneMappingShader      = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_ToneMapping.glsl"             );
		s_PostProcessorData.GaussianBlurShader     = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_GaussianBlur.glsl"            );
		s_PostProcessorData.AdditiveBlendingShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_AdditiveTextureBlending.glsl" );
		s_PostProcessorData.DeferredLightingShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_DeferredLighting.glsl"        );
	}

	void PostProcessor::ApplyPostProcessingEffect(const uint32_t& renderID, const PostProcessingEffect& effect, const Settings& settings)
	{
		ATLAS_PROFILE_FUNCTION();

		s_PostProcessorData.SettingsBuffer.Strength     = settings.Strength;
		s_PostProcessorData.SettingsBuffer.KernelOffset = settings.KernelOffset;
		s_PostProcessorData.SettingsUniformBuffer->SetData(&s_PostProcessorData.SettingsBuffer, sizeof(Settings));

		RenderCommand::BindTextureSlot(0, renderID);

		switch (effect)
		{
		default:
		case Atlas::PostProcessor::PostProcessingEffect::None:
			return;
		case Atlas::PostProcessor::PostProcessingEffect::Inversion:
			s_PostProcessorData.InversionShader->Bind();
			break;
		case Atlas::PostProcessor::PostProcessingEffect::Greyscale:
			s_PostProcessorData.GreyscaleShader->Bind();
			break;
		case Atlas::PostProcessor::PostProcessingEffect::Sharpen:
			s_PostProcessorData.SharpenShader->Bind();
			break;
		case Atlas::PostProcessor::PostProcessingEffect::Blur:
			s_PostProcessorData.BlurShader->Bind();
			break;
		case Atlas::PostProcessor::PostProcessingEffect::EdgeDetection:
			s_PostProcessorData.EdgeDetectionShader->Bind();
			break;
		case Atlas::PostProcessor::PostProcessingEffect::GammaCorrection:
			s_PostProcessorData.GammaCorrectionShader->Bind();
			break;
		case Atlas::PostProcessor::PostProcessingEffect::ToneMapping:
			s_PostProcessorData.ToneMappingShader->Bind();
			break;
		case Atlas::PostProcessor::PostProcessingEffect::Bloom:
			s_PostProcessorData.GaussianBlurShader->Bind();
			break;
		}

		RenderCommand::DrawIndexed(s_PostProcessorData.RenderVertexArray, s_PostProcessorData.RenderIndices);
	}

	void PostProcessor::ApplyAdditiveTextureBlending(const uint32_t& texture1ID, const uint32_t& texture2ID)
	{
		RenderCommand::BindTextureSlot(0, texture1ID);
		RenderCommand::BindTextureSlot(1, texture2ID);

		s_PostProcessorData.AdditiveBlendingShader->Bind();

		RenderCommand::DrawIndexed(s_PostProcessorData.RenderVertexArray, s_PostProcessorData.RenderIndices);
	}

	void PostProcessor::ApplyDeferredShading(const uint32_t& positionTexID, const uint32_t& normalTexID, const uint32_t& albedoTexID, const uint32_t& materialTexID)
	{
		RenderCommand::BindTextureSlot(0, positionTexID);
		RenderCommand::BindTextureSlot(1, normalTexID);
		RenderCommand::BindTextureSlot(2, albedoTexID);
		RenderCommand::BindTextureSlot(3, materialTexID);

		s_PostProcessorData.DeferredLightingShader->Bind();

		RenderCommand::DrawIndexed(s_PostProcessorData.RenderVertexArray, s_PostProcessorData.RenderIndices);
	}
}