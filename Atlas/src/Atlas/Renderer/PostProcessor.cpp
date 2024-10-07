#include "atlaspch.h"
#include "Atlas/Renderer/PostProcessor.h"

#include "Atlas/Renderer/VertexArray.h"
#include "Atlas/Renderer/Buffer.h"
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

		Ref<Shader> InversionShader;
		Ref<Shader> GreyscaleShader;
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

		s_PostProcessorData.InversionShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Inversion.glsl");
		s_PostProcessorData.GreyscaleShader = Shader::Create("assets/shaders/PostProcessing/PP_Vert.glsl", "assets/shaders/PostProcessing/PP_Frag_Greyscale.glsl");
	}

	void PostProcessor::ApplyPostProcessingEffect(PostProcessingEffect effect, uint32_t renderID)
	{
		ATLAS_PROFILE_FUNCTION();

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
		}

		RenderCommand::BindTextureSlot(0, renderID);
		RenderCommand::DrawIndexed(s_PostProcessorData.RenderVertexArray, s_PostProcessorData.RenderIndices);
	}
}