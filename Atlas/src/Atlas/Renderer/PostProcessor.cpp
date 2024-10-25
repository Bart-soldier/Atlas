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

		Ref<Shader> InversionShader;
		Ref<Shader> GreyscaleShader;
		Ref<Shader> NumericalShader;
		Ref<Shader> BlurShader;
		Ref<Shader> EdgeDetectionShader;
		Ref<Shader> GammaCorrectionShader;
		Ref<Shader> ToneMappingShader;
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
			{ ShaderDataType::Float2, "a_Position"  },
			{ ShaderDataType::Float2, "a_TexCoords" }
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
		s_PostProcessorData.InversionShader       = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Inversion.glsl"      );
		s_PostProcessorData.GreyscaleShader       = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Greyscale.glsl"      );
		s_PostProcessorData.NumericalShader       = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Numerical.glsl"      );
		s_PostProcessorData.BlurShader            = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Blur.glsl"           );
		s_PostProcessorData.EdgeDetectionShader   = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_EdgeDetection.glsl"  );
		s_PostProcessorData.GammaCorrectionShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_GammaCorrection.glsl");
		s_PostProcessorData.ToneMappingShader     = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_ToneMapping.glsl"    );
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
		case Atlas::PostProcessor::PostProcessingEffect::Numerical:
			s_PostProcessorData.NumericalShader->Bind();
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
		}

		RenderCommand::DrawIndexed(s_PostProcessorData.RenderVertexArray, s_PostProcessorData.RenderIndices);
	}
}