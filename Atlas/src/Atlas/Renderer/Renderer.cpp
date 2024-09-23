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
			glm::vec3 Position;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		// Lights
		struct LightData
		{
			uint32_t LightCount;

			glm::vec3 AmbientLightColor;
			float AmbientLightIntensity;
		};
		LightData LightBuffer;
		Ref<UniformBuffer> LightUniformBuffer;

		// Storage buffers
		Ref<StorageBuffer> SceneLightPositionsStorageBuffer;
		Ref<StorageBuffer> SceneLightColorsStorageBuffer;
		Ref<StorageBuffer> SceneLightDirectionsStorageBuffer;
		Ref<StorageBuffer> SceneLightIntensitiesStorageBuffer;
		Ref<StorageBuffer> SceneLightAmbientStrengthStorageBuffer;
		Ref<StorageBuffer> SceneLightDiffuseStrengthStorageBuffer;
		Ref<StorageBuffer> SceneLightSpecularStrengthStorageBuffer;

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
		s_Data.MeshIndexBuffer = IndexBuffer::Create(s_Data.MaxIndices);
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
	
		// Uniform & storage buffers
		s_Data.CameraUniformBuffer                     = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
		s_Data.LightUniformBuffer                      = UniformBuffer::Create(sizeof(RendererData::LightData) , 1);
		s_Data.SceneLightPositionsStorageBuffer        = StorageBuffer::Create(0                               , 2);
		s_Data.SceneLightColorsStorageBuffer           = StorageBuffer::Create(0                               , 3);
		s_Data.SceneLightDirectionsStorageBuffer       = StorageBuffer::Create(0                               , 4);
		s_Data.SceneLightIntensitiesStorageBuffer      = StorageBuffer::Create(0                               , 5);
		s_Data.SceneLightAmbientStrengthStorageBuffer  = StorageBuffer::Create(0                               , 6);
		s_Data.SceneLightDiffuseStrengthStorageBuffer  = StorageBuffer::Create(0                               , 7);
		s_Data.SceneLightSpecularStrengthStorageBuffer = StorageBuffer::Create(0                               , 8);
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

	void Renderer::BeginScene(const Camera& camera, const TransformComponent& cameraTransform, const SceneLighting& sceneLighting)
	{
		ATLAS_PROFILE_FUNCTION();

		SetUniformAndStorageBuffers(camera.GetProjection() * glm::inverse(cameraTransform.GetTransform()), cameraTransform.Translation, sceneLighting);

		StartBatch();
	}

	void Renderer::BeginScene(const EditorCamera& camera, const SceneLighting& sceneLighting)
	{
		ATLAS_PROFILE_FUNCTION();

		SetUniformAndStorageBuffers(camera.GetViewProjection(), camera.GetPosition(), sceneLighting);

		StartBatch();
	}

	void Renderer::SetUniformAndStorageBuffers(const glm::mat4& cameraViewProjection, const glm::vec3& cameraPosition, const SceneLighting& sceneLighting)
	{
		// Uniform buffers
		s_Data.CameraBuffer.ViewProjection = cameraViewProjection;
		s_Data.CameraBuffer.Position = cameraPosition;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		if (s_Data.LightBuffer.LightCount != sceneLighting.LightCount)
		{
			s_Data.LightBuffer.LightCount = sceneLighting.LightCount;
			UpdateSceneLightBuffers(sceneLighting.LightCount);
		}
		s_Data.LightBuffer.AmbientLightColor = sceneLighting.AmbientLightColor;
		s_Data.LightBuffer.AmbientLightIntensity = sceneLighting.AmbientLightIntensity;
		s_Data.LightUniformBuffer->SetData(&s_Data.LightBuffer, sizeof(RendererData::LightData));


		// Storage buffers
		s_Data.SceneLightPositionsStorageBuffer       ->SetData(sceneLighting.LightPositions.data()        , sizeof(glm::vec3) * sceneLighting.LightPositions.size()        );
		s_Data.SceneLightColorsStorageBuffer          ->SetData(sceneLighting.LightColors.data()           , sizeof(glm::vec3) * sceneLighting.LightColors.size()           );
		s_Data.SceneLightDirectionsStorageBuffer      ->SetData(sceneLighting.LightDirections.data()       , sizeof(glm::vec4) * sceneLighting.LightDirections.size()       );
		s_Data.SceneLightIntensitiesStorageBuffer     ->SetData(sceneLighting.LightIntensities.data()      , sizeof(float)     * sceneLighting.LightIntensities.size()      );
		s_Data.SceneLightAmbientStrengthStorageBuffer ->SetData(sceneLighting.LightAmbientStrengths.data() , sizeof(float)     * sceneLighting.LightAmbientStrengths.size() );
		s_Data.SceneLightDiffuseStrengthStorageBuffer ->SetData(sceneLighting.LightDiffuseStrengths.data() , sizeof(float)     * sceneLighting.LightDiffuseStrengths.size() );
		s_Data.SceneLightSpecularStrengthStorageBuffer->SetData(sceneLighting.LightSpecularStrengths.data(), sizeof(float)     * sceneLighting.LightSpecularStrengths.size());
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
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.MeshVertexBufferPtr - (uint8_t*)s_Data.MeshVertexBufferBase);
			s_Data.MeshVertexBuffer->SetData(s_Data.MeshVertexBufferBase, dataSize);

			// IBO / EBO
			uint32_t count = (uint32_t)((uint8_t*)s_Data.MeshIndexBufferPtr - (uint8_t*)s_Data.MeshIndexBufferBase);
			s_Data.MeshIndexBuffer->SetData(s_Data.MeshIndexBufferBase, count);

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

	void Renderer::UpdateSceneLightBuffers(uint32_t lightCount)
	{
		s_Data.SceneLightPositionsStorageBuffer       ->SetSize(sizeof(glm::vec3) * lightCount);
		s_Data.SceneLightColorsStorageBuffer          ->SetSize(sizeof(glm::vec3) * lightCount);
		s_Data.SceneLightDirectionsStorageBuffer      ->SetSize(sizeof(glm::vec4) * lightCount);
		s_Data.SceneLightIntensitiesStorageBuffer     ->SetSize(sizeof(float)     * lightCount);
		s_Data.SceneLightAmbientStrengthStorageBuffer ->SetSize(sizeof(float)     * lightCount);
		s_Data.SceneLightDiffuseStrengthStorageBuffer ->SetSize(sizeof(float)     * lightCount);
		s_Data.SceneLightSpecularStrengthStorageBuffer->SetSize(sizeof(float)     * lightCount);
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

	void Renderer::DrawMesh(const glm::mat4& transform, MeshComponent& src, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		size_t vertexCount = src.VertexCount;
		const glm::mat3 normalMatrix = glm::transpose(glm::inverse(transform));

		int diffuseTextureIndex  = EnsureTextureSlot(src.Material.GetDiffuseTexture());
		int specularTextureIndex = EnsureTextureSlot(src.Material.GetSpecularTexture());

		if (s_Data.MeshIndexCount >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		for (size_t i = 0; i < vertexCount; i++)
		{
			int currentIndex = i * 8;

			s_Data.MeshVertexBufferPtr->Position             = transform * glm::vec4({ src.Vertices[currentIndex], src.Vertices[currentIndex + 1], src.Vertices[currentIndex + 2], 1.0f });
			s_Data.MeshVertexBufferPtr->Normal               = normalMatrix * glm::vec3({ src.Vertices[currentIndex + 3], src.Vertices[currentIndex + 4], src.Vertices[currentIndex + 5] });
			s_Data.MeshVertexBufferPtr->TexCoord             = glm::vec2({ src.Vertices[currentIndex + 6], src.Vertices[currentIndex + 7] });

			s_Data.MeshVertexBufferPtr->AmbientColor         = src.Material.GetAmbientColor();
			s_Data.MeshVertexBufferPtr->DiffuseColor         = src.Material.GetDiffuseColor();
			s_Data.MeshVertexBufferPtr->SpecularColor        = src.Material.GetSpecularColor();
			s_Data.MeshVertexBufferPtr->Shininess            = src.Material.GetShininess();

			s_Data.MeshVertexBufferPtr->DiffuseTextureIndex  = diffuseTextureIndex;
			s_Data.MeshVertexBufferPtr->SpecularTextureIndex = specularTextureIndex;
			
			s_Data.MeshVertexBufferPtr->EntityID             = entityID;

			s_Data.MeshVertexBufferPtr++;
		}

		for (uint32_t i = 0; i < src.IndexCount; i++)
		{
			s_Data.MeshIndexBufferPtr->Index = i;
			s_Data.MeshIndexBufferPtr++;
		}

		s_Data.MeshIndexCount += vertexCount;

		s_Data.Stats.MeshCount++;
	}
}