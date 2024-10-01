#include "atlaspch.h"

#include "Atlas/Renderer/Renderer.h"

#include "Atlas/Renderer/VertexArray.h"
#include "Atlas/Renderer/Shader.h"
#include "Atlas/Renderer/UniformBuffer.h"
#include "Atlas/Renderer/StorageBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Atlas
{
	// 2D
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		int TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};

	// 3D
	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;

		glm::vec3 AmbientColor;
		glm::vec3 DiffuseColor;
		glm::vec3 SpecularColor;
		float Shininess;
		uint32_t DiffuseTextureIndex;
		uint32_t SpecularTextureIndex;

		// Editor-only
		int EntityID;
	};

	struct MeshIndex
	{
		uint32_t Index;
	};

	struct RendererData
	{
		// Per draw call
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4; // TODO: Check renderer capabilities
		static const uint32_t MaxIndices = MaxQuads * 6; // TODO: Check renderer capabilities
		static const uint32_t MaxTextureSlots = 32; // TODO: Check renderer capabilities

		// 2D
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		glm::vec4 QuadVertexPositions[4];
		
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineIndexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		// 3D
		Ref<VertexArray> MeshVertexArray;
		Ref<VertexBuffer> MeshVertexBuffer;
		Ref<IndexBuffer> MeshIndexBuffer;
		Ref<Shader> MeshShader;
		
		uint32_t MeshIndexCount = 0;
		MeshVertex* MeshVertexBufferBase = nullptr;
		MeshVertex* MeshVertexBufferPtr = nullptr;
		MeshIndex* MeshIndexBufferBase = nullptr;
		MeshIndex* MeshIndexBufferPtr = nullptr;

		// Textures
		Ref<Texture2D> WhiteTexture;
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		// Camera
		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec4 Position;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		// Lights
		struct LightCountData
		{
			uint32_t LightCount;
		};
		LightCountData LightCountBuffer;
		Ref<UniformBuffer> LightCountUniformBuffer;

		// Storage buffers
		uint32_t LightStorageBufferCapacity = 100;
		Ref<StorageBuffer> LightStorageBuffer;

		// Misc.
		Renderer::Statistics Stats;
		RendererAPI::PolygonMode PolygonMode = RendererAPI::PolygonMode::Fill;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		// Quad VAO
		s_Data.QuadVertexArray = VertexArray::Create();

		// Quad VBO
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Int,    "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		// Quad IBO / EBO
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIndexBuffer);
		delete[] quadIndices;

		// Circle VAO
		s_Data.CircleVertexArray = VertexArray::Create();

		// Circle VBO
		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

		// Circle IBO / EBO
		s_Data.CircleVertexArray->SetIndexBuffer(quadIndexBuffer);

		// Line VAO
		s_Data.LineVertexArray = VertexArray::Create();

		// Line VBO
		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		// Mesh VAO
		s_Data.MeshVertexArray = VertexArray::Create();

		// Mesh VBO
		s_Data.MeshVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(MeshVertex));
		s_Data.MeshVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"             },
			{ ShaderDataType::Float3, "a_Normal"               },
			{ ShaderDataType::Float2, "a_TexCoord"             },
			{ ShaderDataType::Float3, "a_AmbientColor"         },
			{ ShaderDataType::Float3, "a_DiffuseColor"         },
			{ ShaderDataType::Float3, "a_SpecularColor"        },
			{ ShaderDataType::Float,  "a_Shininess"            },
			{ ShaderDataType::UInt,   "a_DiffuseTextureIndex"  },
			{ ShaderDataType::UInt,   "a_SpecularTextureIndex" },
			{ ShaderDataType::Int,    "a_EntityID"             }
			});
		s_Data.MeshVertexArray->AddVertexBuffer(s_Data.MeshVertexBuffer);
		s_Data.MeshVertexBufferBase = new MeshVertex[s_Data.MaxVertices];

		// Mesh IBO / EBO
		s_Data.MeshIndexBuffer = IndexBuffer::Create(s_Data.MaxIndices * sizeof(MeshIndex));
		s_Data.MeshVertexArray->SetIndexBuffer(s_Data.MeshIndexBuffer);
		s_Data.MeshIndexBufferBase = new MeshIndex[s_Data.MaxIndices];

		// Textures
		TextureSpecification whiteTextureSpecification = TextureSpecification();
		whiteTextureSpecification.GenerateMips = false;
		s_Data.WhiteTexture = Texture2D::Create(whiteTextureSpecification);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		// Shaders
		s_Data.QuadShader   = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
		s_Data.LineShader   = Shader::Create("assets/shaders/Renderer2D_Line.glsl");
		s_Data.MeshShader   = Shader::Create("assets/shaders/Renderer3D_Vert.glsl", "assets/shaders/Renderer3D_Frag.glsl");
	
		// Uniform buffers
		s_Data.CameraUniformBuffer     = UniformBuffer::Create(sizeof(RendererData::CameraData)    , 0);
		s_Data.LightCountUniformBuffer = UniformBuffer::Create(sizeof(RendererData::LightCountData), 1);

		// Storage buffers
		s_Data.LightStorageBuffer = StorageBuffer::Create(sizeof(LightData) * s_Data.LightStorageBufferCapacity, 2);
	}

	void Renderer::Shutdown()
	{
		ATLAS_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;

		delete[] s_Data.CircleVertexBufferBase;

		delete[] s_Data.LineVertexBufferBase;

		delete[] s_Data.MeshVertexBufferBase;
		delete[] s_Data.MeshIndexBufferBase;
	}

	void Renderer::BeginScene(const Camera& camera, const TransformComponent& cameraTransform, const std::vector<LightData>& lights)
	{
		ATLAS_PROFILE_FUNCTION();

		SetUniformAndStorageBuffers(camera.GetProjection() * glm::inverse(cameraTransform.GetTransform()), glm::vec4(cameraTransform.Translation, 1.0f), lights);

		StartBatch();
	}

	void Renderer::BeginScene(const EditorCamera& camera, const std::vector<LightData>& lights)
	{
		ATLAS_PROFILE_FUNCTION();

		SetUniformAndStorageBuffers(camera.GetViewProjection(), glm::vec4(camera.GetPosition(), 1.0f), lights);

		StartBatch();
	}

	void Renderer::SetUniformAndStorageBuffers(const glm::mat4& cameraViewProjection, const glm::vec4& cameraPosition, const std::vector<LightData>& lights)
	{
		// Uniform buffers
		s_Data.CameraBuffer.ViewProjection = cameraViewProjection;
		s_Data.CameraBuffer.Position = cameraPosition;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		s_Data.LightCountBuffer.LightCount = lights.size();
		s_Data.LightCountUniformBuffer->SetData(&s_Data.LightCountBuffer, sizeof(RendererData::LightCountData));

		EnsureLightStorageBufferCapacity(lights.capacity());
		s_Data.LightStorageBuffer->SetData(lights.data(), sizeof(LightData) * lights.size());
	}

	void Renderer::EnsureLightStorageBufferCapacity(uint32_t capacity)
	{
		if (capacity <= s_Data.LightStorageBufferCapacity)
		{
			return;
		}

		s_Data.LightStorageBufferCapacity = capacity;
		s_Data.LightStorageBuffer->SetSize(sizeof(LightData) * s_Data.LightStorageBufferCapacity);
	}

	int Renderer::EnsureTextureSlot(const Ref<Texture2D>& texture)
	{
		int textureIndex = 0;

		if (texture == nullptr)
		{
			return textureIndex;
		}

		// Check if texture is already slotted
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture is not found
		if (textureIndex == 0)
		{
			if (s_Data.TextureSlotIndex >= RendererData::MaxTextureSlots)
			{
				NextBatch();
			}

			textureIndex = s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		return textureIndex;
	}

	void Renderer::EndScene()
	{
		ATLAS_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			// VBO
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			// Textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			{
				s_Data.TextureSlots[i]->Bind(i);
			}

			// Shader
			s_Data.QuadShader->Bind();

			// Draw
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount)
		{
			// VBO
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			// Shader
			s_Data.CircleShader->Bind();

			// Draw
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.LineIndexCount)
		{
			// VBO
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			// Shader
			s_Data.LineShader->Bind();

			// Draw
			RenderCommand::SetLineWidth(4.0f);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineIndexCount);
			RenderCommand::SetLineWidth(2.0f);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.MeshIndexCount)
		{
			// VBO
			uint32_t vertexDataSize = (uint32_t)((uint8_t*)s_Data.MeshVertexBufferPtr - (uint8_t*)s_Data.MeshVertexBufferBase);
			s_Data.MeshVertexBuffer->SetData(s_Data.MeshVertexBufferBase, vertexDataSize);

			// IBO / EBO
			uint32_t indexDataSize = (uint32_t)((uint8_t*)s_Data.MeshIndexBufferPtr - (uint8_t*)s_Data.MeshIndexBufferBase);
			s_Data.MeshIndexBuffer->SetData(s_Data.MeshIndexBufferBase, indexDataSize);

			// Textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			{
				s_Data.TextureSlots[i]->Bind(i);
			}

			// Shader
			s_Data.MeshShader->Bind();

			// Draw
			RenderCommand::DrawIndexed(s_Data.MeshVertexArray, s_Data.MeshIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineIndexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.MeshIndexCount = 0;
		s_Data.MeshVertexBufferPtr = s_Data.MeshVertexBufferBase;
		s_Data.MeshIndexBufferPtr = s_Data.MeshIndexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	uint32_t Renderer::GetLightStorageBufferCapacity()
	{
		return s_Data.LightStorageBufferCapacity;
	}

	RendererAPI::PolygonMode Renderer::GetPolygonMode()
	{
		return s_Data.PolygonMode;
	}

	void Renderer::SetPolygonMode(RendererAPI::PolygonMode polygonMode)
	{
		s_Data.PolygonMode = polygonMode;
		RenderCommand::SetPolygonMode(polygonMode);
	}

	void Renderer::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer::Statistics Renderer::GetStats()
	{
		return s_Data.Stats;
	}

	/* --------------- COLOR VERSION --------------- */

	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const int textureIndex = 0; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_Data.QuadIndexCount >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	/* --------------- TEXTURE VERSION --------------- */

	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		if (s_Data.QuadIndexCount >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		int textureIndex = EnsureTextureSlot(texture);

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	/* --------------- SUBTEXTURE VERSION --------------- */

	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subTexture, tilingFactor, tintColor);
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, subTexture, tilingFactor, tintColor);
	}

	void Renderer::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subTexture, tilingFactor, tintColor);
	}

	void Renderer::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, subTexture, tilingFactor, tintColor);
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTexture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const Ref<Texture2D> texture = subTexture->GetTexture();

		if (s_Data.QuadIndexCount >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		int textureIndex = EnsureTextureSlot(texture);

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	/* --------------- EDITOR-USE VERSION --------------- */

	void Renderer::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		switch (src.Type)
		{
			case SpriteRendererComponent::RenderType::Square:
				if (!src.Texture)
				{
					DrawQuad(transform, src.Color, entityID);
				}
				else if (src.SubTexture)
				{
					DrawQuad(transform, src.SubTexture, src.TilingFactor, src.Color, entityID);
				}
				else
				{
					DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
				}
				break;
			case SpriteRendererComponent::RenderType::Circle:
				DrawCircle(transform, src.Color, src.Thickness, src.Fade, entityID);
				break;
		}
	}

	void Renderer::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		if (s_Data.CircleIndexCount >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		if (s_Data.LineIndexCount >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineIndexCount += 2;
	}

	void Renderer::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	void Renderer::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer::DrawMesh(const glm::mat4& transform, MeshComponent& mesh, MaterialComponent* material, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		const glm::mat3 normalMatrix = glm::transpose(glm::inverse(transform));

		int diffuseTextureIndex  = material == nullptr ? 0 : EnsureTextureSlot(material->Material.GetDiffuseTexture());
		int specularTextureIndex = material == nullptr ? 0 : EnsureTextureSlot(material->Material.GetSpecularTexture());

		if (s_Data.MeshIndexCount >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		std::vector<Mesh::Vertex> vertices = mesh.Mesh.GetVertices();

		for (size_t i = 0; i < mesh.Mesh.GetVertices().size(); i++)
		{
			s_Data.MeshVertexBufferPtr->Position             = transform * glm::vec4(vertices[i].Position, 1.0f);
			s_Data.MeshVertexBufferPtr->Normal               = normalMatrix * vertices[i].Normal;
			s_Data.MeshVertexBufferPtr->TexCoord             = vertices[i].TexCoords;

			s_Data.MeshVertexBufferPtr->AmbientColor         = material == nullptr ? glm::vec3(1.0f) : material->Material.GetAmbientColor();
			s_Data.MeshVertexBufferPtr->DiffuseColor         = material == nullptr ? glm::vec3(1.0f) : material->Material.GetDiffuseColor();
			s_Data.MeshVertexBufferPtr->SpecularColor        = material == nullptr ? glm::vec3(1.0f) : material->Material.GetSpecularColor();
			s_Data.MeshVertexBufferPtr->Shininess            = material == nullptr ? 0.25f           : material->Material.GetShininess();

			s_Data.MeshVertexBufferPtr->DiffuseTextureIndex  = diffuseTextureIndex;
			s_Data.MeshVertexBufferPtr->SpecularTextureIndex = specularTextureIndex;
			
			s_Data.MeshVertexBufferPtr->EntityID             = entityID;

			s_Data.MeshVertexBufferPtr++;
		}

		std::vector<uint32_t> indices = mesh.Mesh.GetIndices();

		for (uint32_t i = 0; i < indices.size(); i++)
		{
			s_Data.MeshIndexBufferPtr->Index = indices[i];
			s_Data.MeshIndexBufferPtr++;
		}

		s_Data.MeshIndexCount += indices.size();

		s_Data.Stats.MeshCount++;
	}
}