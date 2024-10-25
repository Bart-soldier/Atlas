#include "atlaspch.h"
#include "Atlas/Renderer/Renderer.h"

#include <Atlas/Core/Application.h>

#include "Atlas/Renderer/Framebuffer.h"
#include "Atlas/Renderer/VertexArray.h"
#include "Atlas/Renderer/Shader.h"
#include "Atlas/Renderer/UniformBuffer.h"
#include "Atlas/Renderer/StorageBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Atlas
{
	struct SimpleVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};

	// 2D
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		uint32_t TexIndex;
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

	// 3D
	struct MeshVertex
	{
		glm::vec3 Position;
		float EntityID;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec3 Tangent;

		glm::mat4 Model;

		glm::vec3 AmbientColor;
		glm::vec3 DiffuseColor;
		glm::vec3 SpecularColor;
		float Shininess;
		uint32_t DiffuseTextureIndex;
		uint32_t SpecularTextureIndex;
		uint32_t NormalMapTextureIndex;
		uint32_t HeightMapTextureIndex;
	};

	struct RendererData
	{
		Ref<Framebuffer> RenderFramebuffer;

		// Per draw call
		static const uint32_t MaxTriangles = 20000;
		static const uint32_t MaxVertices     = MaxTriangles * 3; // TODO: Check renderer capabilities
		static const uint32_t MaxIndices      = MaxTriangles * 3; // TODO: Check renderer capabilities
		static const uint32_t MaxQuads = MaxTriangles / 2;
		static const uint32_t MaxQuadVertices = MaxQuads * 4; // TODO: Check renderer capabilities
		static const uint32_t MaxQuadIndices  = MaxQuads * 6; // TODO: Check renderer capabilities
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

		uint32_t QuadVertexCount = 0;
		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;

		uint32_t CircleVertexCount = 0;
		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;

		uint32_t LineVertexCount = 0;
		uint32_t LineIndexCount = 0;
		SimpleVertex* LineVertexBufferBase = nullptr;

		// 3D
		Ref<VertexArray> MeshVertexArray;
		Ref<VertexBuffer> MeshVertexBuffer;
		Ref<IndexBuffer> MeshIndexBuffer;
		Ref<Shader> MeshShader;
		
		uint32_t MeshVertexCount = 0;
		uint32_t MeshIndexCount = 0;
		MeshVertex* MeshVertexBufferBase = nullptr;
		uint32_t* MeshIndexBufferBase = nullptr;

		// Outline
		Ref<VertexArray> MeshOutlineVertexArray;
		Ref<VertexBuffer> MeshOutlineVertexBuffer;
		Ref<IndexBuffer> MeshOutlineIndexBuffer;
		Ref<Shader> MeshOutlineShader;

		uint32_t MeshOutlineVertexCount = 0;
		uint32_t MeshOutlineIndexCount = 0;
		SimpleVertex* MeshOutlineVertexBufferBase = nullptr;
		uint32_t* MeshOutlineIndexBufferBase = nullptr;

		// Skybox
		Ref<VertexArray> SkyboxVertexArray;
		Ref<Shader> SkyboxShader;

		uint32_t SkyboxVertexCount = 24;
		uint32_t SkyboxIndexCount = 36;

		// Textures
		Ref<Texture2D> WhiteTexture;
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		// Camera
		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::mat4 Projection;
			glm::mat4 View;
			glm::vec4 Position;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		// Lights
		uint32_t LightCountBuffer;
		Ref<UniformBuffer> LightCountUniformBuffer;

		// Storage buffers
		uint32_t LightStorageBufferCapacity = 100;
		Ref<StorageBuffer> LightStorageBuffer;

		// Graphics Settings
		struct Settings
		{
			float Gamma = 2.2f;
			float Exposure = 1.0f;
			float ParallaxScale = 0.1f;
		};
		Settings SettingsBuffer;
		Ref<UniformBuffer> SettingsUniformBuffer;

		// Misc.
		Renderer::Statistics Stats;
		RendererAPI::PolygonMode PolygonMode = RendererAPI::PolygonMode::Fill;
	};

	static RendererData s_RendererData;

	void Renderer::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		InitArrays();
		InitSkybox();
		InitTexture();
		InitShaders();
		InitBuffers();
	}

	void Renderer::InitArrays()
	{
		FramebufferSpecification fbSpec;
		// Render FB: Color, EntityID, PostProcessing, Depth
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::DEPTH24_STENCIL8 };
		Application& app = Application::Get();
		fbSpec.Width  = app.GetWindow().GetWidth();
		fbSpec.Height = app.GetWindow().GetHeight();
		s_RendererData.RenderFramebuffer = Framebuffer::Create(fbSpec);

		// Quad VAO
		s_RendererData.QuadVertexArray = VertexArray::Create();

		// Quad VBO
		s_RendererData.QuadVertexBuffer = VertexBuffer::Create(s_RendererData.MaxQuadVertices * sizeof(QuadVertex));
		s_RendererData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::UInt,   "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
		});
		s_RendererData.QuadVertexArray->AddVertexBuffer(s_RendererData.QuadVertexBuffer);
		s_RendererData.QuadVertexBufferBase = new QuadVertex[s_RendererData.MaxQuadVertices];

		s_RendererData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_RendererData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		// Quad IBO / EBO
		uint32_t* quadIndices = new uint32_t[s_RendererData.MaxQuadIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_RendererData.MaxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices, s_RendererData.MaxQuadIndices * sizeof(uint32_t));
		s_RendererData.QuadVertexArray->SetIndexBuffer(quadIndexBuffer);
		delete[] quadIndices;

		// Circle VAO
		s_RendererData.CircleVertexArray = VertexArray::Create();

		// Circle VBO
		s_RendererData.CircleVertexBuffer = VertexBuffer::Create(s_RendererData.MaxQuadVertices * sizeof(CircleVertex));
		s_RendererData.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
			});
		s_RendererData.CircleVertexArray->AddVertexBuffer(s_RendererData.CircleVertexBuffer);
		s_RendererData.CircleVertexBufferBase = new CircleVertex[s_RendererData.MaxQuadVertices];

		// Circle IBO / EBO
		s_RendererData.CircleVertexArray->SetIndexBuffer(quadIndexBuffer);

		// Line VAO
		s_RendererData.LineVertexArray = VertexArray::Create();

		// Line VBO
		s_RendererData.LineVertexBuffer = VertexBuffer::Create(s_RendererData.MaxVertices * sizeof(SimpleVertex));
		s_RendererData.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_RendererData.LineVertexArray->AddVertexBuffer(s_RendererData.LineVertexBuffer);
		s_RendererData.LineVertexBufferBase = new SimpleVertex[s_RendererData.MaxVertices];

		// Mesh VAO
		s_RendererData.MeshVertexArray = VertexArray::Create();

		// Mesh VBO
		s_RendererData.MeshVertexBuffer = VertexBuffer::Create(s_RendererData.MaxVertices * sizeof(MeshVertex));
		s_RendererData.MeshVertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_PositionID"            }, // xyz is Position and w is EntityID (editor-only)
			{ ShaderDataType::Float3, "a_Normal"                },
			{ ShaderDataType::Float2, "a_TexCoord"              },
			{ ShaderDataType::Float3, "a_Tangent"               },
			{ ShaderDataType::Mat4,   "a_Model"                 },
			{ ShaderDataType::Float3, "a_AmbientColor"          },
			{ ShaderDataType::Float3, "a_DiffuseColor"          },
			{ ShaderDataType::Float3, "a_SpecularColor"         },
			{ ShaderDataType::Float,  "a_Shininess"             },
			{ ShaderDataType::UInt,   "a_DiffuseTextureIndex"   },
			{ ShaderDataType::UInt,   "a_SpecularTextureIndex"  },
			{ ShaderDataType::UInt,   "a_NormalMapTextureIndex" },
			{ ShaderDataType::UInt,   "a_HeightMapTextureIndex" }
			});
		s_RendererData.MeshVertexArray->AddVertexBuffer(s_RendererData.MeshVertexBuffer);
		s_RendererData.MeshVertexBufferBase = new MeshVertex[s_RendererData.MaxVertices];

		// Mesh IBO / EBO
		s_RendererData.MeshIndexBuffer = IndexBuffer::Create(s_RendererData.MaxIndices * sizeof(uint32_t));
		s_RendererData.MeshVertexArray->SetIndexBuffer(s_RendererData.MeshIndexBuffer);
		s_RendererData.MeshIndexBufferBase = new uint32_t[s_RendererData.MaxIndices];

		// Outline VAO
		s_RendererData.MeshOutlineVertexArray = VertexArray::Create();

		// Outline VBO
		s_RendererData.MeshOutlineVertexBuffer = VertexBuffer::Create(s_RendererData.MaxVertices * sizeof(SimpleVertex));
		s_RendererData.MeshOutlineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_RendererData.MeshOutlineVertexArray->AddVertexBuffer(s_RendererData.MeshOutlineVertexBuffer);
		s_RendererData.MeshOutlineVertexBufferBase = new SimpleVertex[s_RendererData.MaxVertices];

		// Outline IBO / EBO
		s_RendererData.MeshOutlineIndexBuffer = IndexBuffer::Create(s_RendererData.MaxIndices * sizeof(uint32_t));
		s_RendererData.MeshOutlineVertexArray->SetIndexBuffer(s_RendererData.MeshOutlineIndexBuffer);
		s_RendererData.MeshOutlineIndexBufferBase = new uint32_t[s_RendererData.MaxIndices];
	}

	void Renderer::InitSkybox()
	{
		// Skybox VAO
		s_RendererData.SkyboxVertexArray = VertexArray::Create();
		glm::vec3* skyboxVertices = new glm::vec3[s_RendererData.SkyboxVertexCount];
		uint32_t* skyboxIndices = new uint32_t[s_RendererData.SkyboxIndexCount];

		// Back face
		skyboxVertices[0] = { -1.0f, -1.0f, -1.0f }; // 0: Bottom-left
		skyboxVertices[1] = {  1.0f,  1.0f, -1.0f }; // 1: Top-right
		skyboxVertices[2] = {  1.0f, -1.0f, -1.0f }; // 2: Bottom-right
		skyboxVertices[3] = { -1.0f,  1.0f, -1.0f }; // 3: Top-left
		skyboxIndices[0] = 0;
		skyboxIndices[1] = 1;
		skyboxIndices[2] = 2;
		skyboxIndices[3] = 3;
		skyboxIndices[4] = 1;
		skyboxIndices[5] = 0;

		// Front face
		skyboxVertices[4] = { -1.0f, -1.0f,  1.0f }; // 0: Bottom-left
		skyboxVertices[5] = {  1.0f,  1.0f,  1.0f }; // 1: Top-right
		skyboxVertices[6] = {  1.0f, -1.0f,  1.0f }; // 2: Bottom-right
		skyboxVertices[7] = { -1.0f,  1.0f,  1.0f }; // 3: Top-left
		skyboxIndices[6] = 4;
		skyboxIndices[7] = 6;
		skyboxIndices[8] = 5;
		skyboxIndices[9] = 7;
		skyboxIndices[10] = 4;
		skyboxIndices[11] = 5;

		// Left face
		skyboxVertices[8]  = { -1.0f, -1.0f, -1.0f }; // 0: Bottom-left
		skyboxVertices[9]  = { -1.0f,  1.0f,  1.0f }; // 1: Top-right
		skyboxVertices[10] = { -1.0f, -1.0f,  1.0f }; // 2: Bottom-right
		skyboxVertices[11] = { -1.0f,  1.0f, -1.0f }; // 3: Top-left
		skyboxIndices[12] = 8;
		skyboxIndices[13] = 10;
		skyboxIndices[14] = 9;
		skyboxIndices[15] = 11;
		skyboxIndices[16] = 8;
		skyboxIndices[17] = 9;

		// Right face
		skyboxVertices[12] = { 1.0f, -1.0f, -1.0f }; // 0: Bottom-left
		skyboxVertices[13] = { 1.0f,  1.0f,  1.0f }; // 1: Top-right
		skyboxVertices[14] = { 1.0f, -1.0f,  1.0f }; // 2: Bottom-right
		skyboxVertices[15] = { 1.0f,  1.0f, -1.0f }; // 3: Top-left
		skyboxIndices[18] = 12;
		skyboxIndices[19] = 13;
		skyboxIndices[20] = 14;
		skyboxIndices[21] = 15;
		skyboxIndices[22] = 13;
		skyboxIndices[23] = 12;

		// Bottom face
		skyboxVertices[16] = {  1.0f, -1.0f,  1.0f }; // 0: Bottom-left
		skyboxVertices[17] = { -1.0f, -1.0f, -1.0f }; // 1: Top-right
		skyboxVertices[18] = { -1.0f, -1.0f,  1.0f }; // 2: Bottom-right
		skyboxVertices[19] = {  1.0f, -1.0f, -1.0f }; // 3: Top-left
		skyboxIndices[24] = 16;
		skyboxIndices[25] = 18;
		skyboxIndices[26] = 17;
		skyboxIndices[27] = 19;
		skyboxIndices[28] = 16;
		skyboxIndices[29] = 17;

		// Top face
		skyboxVertices[20] = {  1.0f, 1.0f,  1.0f }; // 0: Bottom-left
		skyboxVertices[21] = { -1.0f, 1.0f, -1.0f }; // 1: Top-right
		skyboxVertices[22] = { -1.0f, 1.0f,  1.0f }; // 2: Bottom-right
		skyboxVertices[23] = {  1.0f, 1.0f, -1.0f }; // 3: Top-left
		skyboxIndices[30] = 20;
		skyboxIndices[31] = 21;
		skyboxIndices[32] = 22;
		skyboxIndices[33] = 23;
		skyboxIndices[34] = 21;
		skyboxIndices[35] = 20;

		// Skybox VBO
		Ref<VertexBuffer> skyboxVertexBuffer = VertexBuffer::Create(skyboxVertices, s_RendererData.SkyboxVertexCount * sizeof(glm::vec3));
		skyboxVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
		});
		s_RendererData.SkyboxVertexArray->AddVertexBuffer(skyboxVertexBuffer);
		delete[] skyboxVertices;

		// Skybox IBO / EBO
		Ref<IndexBuffer> skyboxIndexBuffer = IndexBuffer::Create(skyboxIndices, s_RendererData.SkyboxIndexCount * sizeof(uint32_t));
		s_RendererData.SkyboxVertexArray->SetIndexBuffer(skyboxIndexBuffer);
		delete[] skyboxIndices;
	}

	void Renderer::InitTexture()
	{
		TextureSpecification whiteTextureSpecification = TextureSpecification();
		whiteTextureSpecification.GenerateMips = false;
		s_RendererData.WhiteTexture = Texture2D::Create(whiteTextureSpecification);
		uint32_t whiteTextureData = 0xffffffff;
		s_RendererData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_RendererData.TextureSlots[0] = s_RendererData.WhiteTexture;
	}

	void Renderer::InitShaders()
	{
		s_RendererData.QuadShader        = Shader::Create("assets/shaders/2D/Renderer2D_Quad.glsl");
		s_RendererData.CircleShader      = Shader::Create("assets/shaders/2D/Renderer2D_Circle.glsl");
		s_RendererData.LineShader        = Shader::Create("assets/shaders/2D/Renderer2D_Line.glsl");
		//s_RendererData.MeshShader        = Shader::Create("assets/shaders/3D/Renderer3D_Vert.glsl", "assets/shaders/3D/Renderer3D_Frag.glsl");
		s_RendererData.MeshShader        = Shader::Create("assets/shaders/3D/Renderer3D_Vert.glsl", "assets/shaders/3D/Renderer3D_FragFlat.glsl");
		s_RendererData.MeshOutlineShader = Shader::Create("assets/shaders/3D/Renderer3D_Outline.glsl");
		s_RendererData.SkyboxShader      = Shader::Create("assets/shaders/Skybox/Skybox_Vert.glsl", "assets/shaders/Skybox/Skybox_Frag.glsl");
	}

	void Renderer::InitBuffers()
	{
		// Uniform buffers
		s_RendererData.SettingsUniformBuffer   = UniformBuffer::Create(sizeof(RendererData::Settings),   0);
		s_RendererData.CameraUniformBuffer     = UniformBuffer::Create(sizeof(RendererData::CameraData), 1);
		s_RendererData.LightCountUniformBuffer = UniformBuffer::Create(sizeof(uint32_t),                 2);

		// Storage buffers
		s_RendererData.LightStorageBuffer = StorageBuffer::Create(sizeof(LightData) * s_RendererData.LightStorageBufferCapacity, 0);
	}

	void Renderer::Shutdown()
	{
		ATLAS_PROFILE_FUNCTION();

		delete[] s_RendererData.QuadVertexBufferBase;

		delete[] s_RendererData.CircleVertexBufferBase;

		delete[] s_RendererData.LineVertexBufferBase;

		delete[] s_RendererData.MeshVertexBufferBase;
		delete[] s_RendererData.MeshIndexBufferBase;

		delete[] s_RendererData.MeshOutlineVertexBufferBase;
		delete[] s_RendererData.MeshOutlineIndexBufferBase;
	}

	void Renderer::EnableStencilWriting()
	{
		RenderCommand::SetStencilFunction(RendererAPI::TestFunction::Always, 1, 0xFF);
		RenderCommand::SetStencilMask(0xFF);
	}

	void Renderer::DisableStencilWriting()
	{
		RenderCommand::SetStencilMask(0x00);
	}

	uint32_t Renderer::GetLightStorageBufferCapacity()
	{
		return s_RendererData.LightStorageBufferCapacity;
	}

	RendererAPI::PolygonMode Renderer::GetPolygonMode()
	{
		return s_RendererData.PolygonMode;
	}

	void Renderer::SetPolygonMode(RendererAPI::PolygonMode polygonMode)
	{
		s_RendererData.PolygonMode = polygonMode;
		RenderCommand::SetPolygonMode(polygonMode);
	}

	const float& Renderer::GetGamma()
	{
		return s_RendererData.SettingsBuffer.Gamma;
	}

	void Renderer::SetGamma(float gamma)
	{
		s_RendererData.SettingsBuffer.Gamma = gamma;
	}

	const float& Renderer::GetExposure()
	{
		return s_RendererData.SettingsBuffer.Exposure;
	}

	void Renderer::SetExposure(float exposure)
	{
		s_RendererData.SettingsBuffer.Exposure = exposure;
	}

	const float& Renderer::GetParallaxScale()
	{
		return s_RendererData.SettingsBuffer.ParallaxScale;
	}

	void Renderer::SetParallaxScale(float scale)
	{
		s_RendererData.SettingsBuffer.ParallaxScale = scale;
	}

	void Renderer::BeginRenderPass()
	{
		s_RendererData.RenderFramebuffer->Bind();
		// TODO: Link to palette
		RenderCommand::SetClearColor({ 0.090f, 0.114f, 0.133f, 1.0f });
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1
		s_RendererData.RenderFramebuffer->ClearAttachment(1, -1);
	}

	void Renderer::EndRenderPass()
	{
		s_RendererData.RenderFramebuffer->Unbind();
	}

	bool Renderer::ResizeFramebuffer(uint32_t width, uint32_t height)
	{
		bool resized = false;
		FramebufferSpecification spec = s_RendererData.RenderFramebuffer->GetSpecification();

		// Zero sized framebuffer is invalid
		if (width > 0.0f && height > 0.0f && (spec.Width != width || spec.Height != height))
		{
			s_RendererData.RenderFramebuffer->Resize(width, height);
			resized = true;
		}

		return resized;
	}

	uint32_t Renderer::GetRenderID()
	{
		return s_RendererData.RenderFramebuffer->GetColorAttachmentRendererID();
	}

	uint32_t Renderer::GetPostProcessRenderID()
	{
		return s_RendererData.RenderFramebuffer->GetColorAttachmentRendererID(2);
	}

	int Renderer::GetEntityIDFromPixel(int x, int y)
	{
		return s_RendererData.RenderFramebuffer->ReadPixel(1, x, y);
	}

	void Renderer::ResetStats()
	{
		memset(&s_RendererData.Stats, 0, sizeof(Statistics));
	}

	Renderer::Statistics Renderer::GetStats()
	{
		return s_RendererData.Stats;
	}

	void Renderer::BeginScene(const Camera& camera, const TransformComponent& cameraTransform, const std::vector<LightData>& lights)
	{
		ATLAS_PROFILE_FUNCTION();

		SetUniformBuffers(camera.GetProjection(), glm::inverse(cameraTransform.GetTransform()), glm::vec4(cameraTransform.Translation, 1.0f));
		SetStorageBuffers(lights);

		StartBatch();
	}

	void Renderer::BeginScene(const EditorCamera& camera, const std::vector<LightData>& lights)
	{
		ATLAS_PROFILE_FUNCTION();

		SetUniformBuffers(camera.GetProjection(), camera.GetViewMatrix(), glm::vec4(camera.GetPosition(), 1.0f));
		SetStorageBuffers(lights);

		StartBatch();
	}

	void Renderer::SetUniformBuffers(const glm::mat4& cameraProjection, const glm::mat4& cameraView, const glm::vec4& cameraPosition)
	{
		s_RendererData.SettingsUniformBuffer->SetData(&s_RendererData.SettingsBuffer, sizeof(RendererData::Settings));

		s_RendererData.CameraBuffer.ViewProjection = cameraProjection * cameraView;
		s_RendererData.CameraBuffer.Projection     = cameraProjection;
		s_RendererData.CameraBuffer.View           = cameraView;
		s_RendererData.CameraBuffer.Position       = cameraPosition;
		s_RendererData.CameraUniformBuffer->SetData(&s_RendererData.CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::SetStorageBuffers(const std::vector<LightData>& lights)
	{
		s_RendererData.LightCountBuffer = lights.size();
		s_RendererData.LightCountUniformBuffer->SetData(&s_RendererData.LightCountBuffer, sizeof(uint32_t));

		EnsureLightStorageBufferCapacity(lights.capacity());
		s_RendererData.LightStorageBuffer->SetData(lights.data(), sizeof(LightData) * lights.size());
	}

	void Renderer::EnsureLightStorageBufferCapacity(uint32_t capacity)
	{
		if (capacity <= s_RendererData.LightStorageBufferCapacity)
		{
			return;
		}

		s_RendererData.LightStorageBufferCapacity = capacity;
		s_RendererData.LightStorageBuffer->SetSize(sizeof(LightData) * s_RendererData.LightStorageBufferCapacity);
	}

	uint32_t Renderer::EnsureTextureSlot(const Ref<Texture2D>& texture)
	{
		uint32_t textureIndex = 0;

		if (texture == nullptr)
		{
			return textureIndex;
		}

		// Check if texture is already slotted
		for (uint32_t i = 1; i < s_RendererData.TextureSlotIndex; i++)
		{
			if (*s_RendererData.TextureSlots[i] == *texture)
			{
				textureIndex = i;
				break;
			}
		}

		// If texture is not found
		if (textureIndex == 0)
		{
			if (s_RendererData.TextureSlotIndex >= RendererData::MaxTextureSlots)
			{
				NextBatch();
			}

			textureIndex = s_RendererData.TextureSlotIndex;
			s_RendererData.TextureSlots[s_RendererData.TextureSlotIndex] = texture;
			s_RendererData.TextureSlotIndex++;
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
		if (s_RendererData.QuadIndexCount)
		{
			// VBO
			s_RendererData.QuadVertexBuffer->SetData(s_RendererData.QuadVertexBufferBase, sizeof(QuadVertex) * s_RendererData.QuadVertexCount);

			// Textures
			for (uint32_t i = 0; i < s_RendererData.TextureSlotIndex; i++)
			{
				s_RendererData.TextureSlots[i]->Bind(i);
			}

			// Shader
			s_RendererData.QuadShader->Bind();

			// Draw
			RenderCommand::DrawIndexed(s_RendererData.QuadVertexArray, s_RendererData.QuadIndexCount);
			s_RendererData.Stats.DrawCalls++;
		}

		if (s_RendererData.CircleIndexCount)
		{
			// VBO
			s_RendererData.CircleVertexBuffer->SetData(s_RendererData.CircleVertexBufferBase, sizeof(CircleVertex) * s_RendererData.CircleVertexCount);

			// Shader
			s_RendererData.CircleShader->Bind();

			// Draw
			RenderCommand::DrawIndexed(s_RendererData.CircleVertexArray, s_RendererData.CircleIndexCount);
			s_RendererData.Stats.DrawCalls++;
		}

		if (s_RendererData.LineIndexCount)
		{
			// VBO
			s_RendererData.LineVertexBuffer->SetData(s_RendererData.LineVertexBufferBase, sizeof(SimpleVertex) * s_RendererData.LineVertexCount);

			// Shader
			s_RendererData.LineShader->Bind();

			// Draw
			RenderCommand::SetLineWidth(4.0f);
			RenderCommand::DrawLines(s_RendererData.LineVertexArray, s_RendererData.LineIndexCount);
			RenderCommand::SetLineWidth(2.0f);
			s_RendererData.Stats.DrawCalls++;
		}

		if (s_RendererData.MeshIndexCount)
		{
			RenderCommand::EnableBackCulling();

			// VBO
			s_RendererData.MeshVertexBuffer->SetData(s_RendererData.MeshVertexBufferBase, sizeof(MeshVertex) * s_RendererData.MeshVertexCount);

			// IBO / EBO
			s_RendererData.MeshIndexBuffer->SetData(s_RendererData.MeshIndexBufferBase, sizeof(uint32_t) * s_RendererData.MeshIndexCount);

			// Textures
			for (uint32_t i = 0; i < s_RendererData.TextureSlotIndex; i++)
			{
				s_RendererData.TextureSlots[i]->Bind(i);
			}

			// Shader
			s_RendererData.MeshShader->Bind();

			// Draw
			RenderCommand::DrawIndexed(s_RendererData.MeshVertexArray, s_RendererData.MeshIndexCount);
			s_RendererData.Stats.DrawCalls++;

			RenderCommand::DisableBackCulling();
		}

		if (s_RendererData.MeshOutlineIndexCount)
		{
			RenderCommand::EnableBackCulling();

			// VBO
			s_RendererData.MeshOutlineVertexBuffer->SetData(s_RendererData.MeshOutlineVertexBufferBase, sizeof(SimpleVertex) * s_RendererData.MeshOutlineVertexCount);

			// IBO / EBO
			s_RendererData.MeshOutlineIndexBuffer->SetData(s_RendererData.MeshOutlineIndexBufferBase, sizeof(uint32_t) * s_RendererData.MeshOutlineIndexCount);

			// Shader
			s_RendererData.MeshOutlineShader->Bind();

			// Draw
			RenderCommand::DrawIndexed(s_RendererData.MeshOutlineVertexArray, s_RendererData.MeshOutlineIndexCount);
			s_RendererData.Stats.DrawCalls++;

			RenderCommand::DisableBackCulling();
		}
	}

	void Renderer::StartBatch()
	{
		s_RendererData.QuadVertexCount = 0;
		s_RendererData.QuadIndexCount = 0;

		s_RendererData.CircleVertexCount = 0;
		s_RendererData.CircleIndexCount = 0;

		s_RendererData.LineVertexCount = 0;
		s_RendererData.LineIndexCount = 0;

		s_RendererData.MeshVertexCount = 0;
		s_RendererData.MeshIndexCount  = 0;

		s_RendererData.MeshOutlineVertexCount = 0;
		s_RendererData.MeshOutlineIndexCount = 0;

		s_RendererData.TextureSlotIndex = 1;
	}

	void Renderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer::BeginPostProcessing()
	{
		RenderCommand::SetPolygonMode(RendererAPI::PolygonMode::Fill);
		RenderCommand::DisableDepthTest();
	}

	void Renderer::EndPostProcessing()
	{
		//PostProcessor::ApplyPostProcessingEffect(Renderer::GetPostProcessRenderID(), PostProcessor::PostProcessingEffect::ToneMapping, Renderer::GetExposure());
		PostProcessor::ApplyPostProcessingEffect(Renderer::GetRenderID(), PostProcessor::PostProcessingEffect::GammaCorrection, Renderer::GetGamma());

		RenderCommand::EnableDepthTest();
		RenderCommand::SetPolygonMode(Renderer::GetPolygonMode());
	}

	void Renderer::DrawPostProcessing(PostProcessorComponent* postProcessor)
	{
		if (postProcessor)
		{
			for (int i = 0; i < postProcessor->Effects.size(); i++)
			{
				PostProcessor::ApplyPostProcessingEffect(Renderer::GetPostProcessRenderID(), postProcessor->Effects[i], { 1.0f, postProcessor->KernelOffsets[i] });
			}
		}
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
		constexpr size_t quadIndexCount  = 6;
		const uint32_t textureIndex = 0; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_RendererData.QuadVertexCount + quadVertexCount >= RendererData::MaxVertices ||
			s_RendererData.QuadIndexCount  + quadIndexCount  >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].Position     = transform * s_RendererData.QuadVertexPositions[i];
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].Color        = color;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TexCoord     = textureCoords[i];
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TexIndex     = textureIndex;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TilingFactor = tilingFactor;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].EntityID     = entityID;
			s_RendererData.QuadVertexCount++;
		}

		s_RendererData.QuadIndexCount += quadIndexCount;

		s_RendererData.Stats.QuadCount++;
		s_RendererData.Stats.TotalVertexCount += quadVertexCount;
		s_RendererData.Stats.TotalIndexCount  += quadIndexCount;
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
		constexpr size_t quadIndexCount  = 6;
		constexpr glm::vec2 textureCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		if (s_RendererData.QuadVertexCount + quadVertexCount >= RendererData::MaxVertices ||
			s_RendererData.QuadIndexCount  + quadIndexCount  >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		uint32_t textureIndex = EnsureTextureSlot(texture);

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].Position     = transform * s_RendererData.QuadVertexPositions[i];
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].Color        = tintColor;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TexCoord     = textureCoords[i];
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TexIndex     = textureIndex;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TilingFactor = tilingFactor;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].EntityID     = entityID;
			s_RendererData.QuadVertexCount++;
		}

		s_RendererData.QuadIndexCount += quadIndexCount;

		s_RendererData.Stats.QuadCount++;
		s_RendererData.Stats.TotalVertexCount += quadVertexCount;
		s_RendererData.Stats.TotalIndexCount  += quadIndexCount;
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
		constexpr size_t quadIndexCount  = 6;
		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const Ref<Texture2D> texture = subTexture->GetTexture();

		if (s_RendererData.QuadVertexCount + quadVertexCount >= RendererData::MaxVertices ||
			s_RendererData.QuadIndexCount  + quadIndexCount  >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		uint32_t textureIndex = EnsureTextureSlot(texture);

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].Position     = transform * s_RendererData.QuadVertexPositions[i];
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].Color        = tintColor;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TexCoord     = textureCoords[i];
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TexIndex     = textureIndex;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].TilingFactor = tilingFactor;
			s_RendererData.QuadVertexBufferBase[s_RendererData.QuadVertexCount].EntityID     = entityID;
			s_RendererData.QuadVertexCount++;
		}

		s_RendererData.QuadIndexCount += quadIndexCount;

		s_RendererData.Stats.QuadCount++;
		s_RendererData.Stats.TotalVertexCount += quadVertexCount;
		s_RendererData.Stats.TotalIndexCount  += quadIndexCount;
	}

	/* --------------- EDITOR-USE VERSION --------------- */

	void Renderer::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, int entityID)
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

		constexpr size_t circleVertexCount = 4;
		constexpr size_t circleIndexCount  = 6;

		if (s_RendererData.CircleVertexCount + circleVertexCount >= RendererData::MaxVertices ||
			s_RendererData.CircleIndexCount  + circleIndexCount  >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		for (size_t i = 0; i < circleVertexCount; i++)
		{
			s_RendererData.CircleVertexBufferBase[s_RendererData.CircleVertexCount].WorldPosition = transform * s_RendererData.QuadVertexPositions[i];
			s_RendererData.CircleVertexBufferBase[s_RendererData.CircleVertexCount].LocalPosition = s_RendererData.QuadVertexPositions[i] * 2.0f;
			s_RendererData.CircleVertexBufferBase[s_RendererData.CircleVertexCount].Color         = color;
			s_RendererData.CircleVertexBufferBase[s_RendererData.CircleVertexCount].Thickness     = thickness;
			s_RendererData.CircleVertexBufferBase[s_RendererData.CircleVertexCount].Fade          = fade;
			s_RendererData.CircleVertexBufferBase[s_RendererData.CircleVertexCount].EntityID      = entityID;
			s_RendererData.CircleVertexCount++;
		}

		s_RendererData.CircleIndexCount += circleIndexCount;

		s_RendererData.Stats.CircleCount++;
		s_RendererData.Stats.TotalVertexCount += circleVertexCount;
		s_RendererData.Stats.TotalIndexCount  += circleIndexCount;
	}

	void Renderer::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		constexpr size_t lineVertexCount = 2;
		constexpr size_t lineIndexCount  = 2;

		if (s_RendererData.LineVertexCount + lineVertexCount >= RendererData::MaxVertices ||
			s_RendererData.LineIndexCount  + lineIndexCount  >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		s_RendererData.LineVertexBufferBase[s_RendererData.LineVertexCount].Position = p0;
		s_RendererData.LineVertexBufferBase[s_RendererData.LineVertexCount].Color    = color;
		s_RendererData.LineVertexBufferBase[s_RendererData.LineVertexCount].EntityID = entityID;
		s_RendererData.LineVertexCount++;

		s_RendererData.LineVertexBufferBase[s_RendererData.LineVertexCount].Position = p1;
		s_RendererData.LineVertexBufferBase[s_RendererData.LineVertexCount].Color    = color;
		s_RendererData.LineVertexBufferBase[s_RendererData.LineVertexCount].EntityID = entityID;
		s_RendererData.LineVertexCount++;

		s_RendererData.LineIndexCount += lineIndexCount;

		s_RendererData.Stats.LineCount++;
		s_RendererData.Stats.TotalVertexCount += lineVertexCount;
		s_RendererData.Stats.TotalIndexCount  += lineIndexCount;
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
			lineVertices[i] = transform * s_RendererData.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer::DrawMesh(const glm::mat4& transform, const MeshComponent& mesh, const MaterialComponent* material, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		const glm::mat3 normalMatrix = glm::transpose(glm::inverse(transform));

		uint32_t diffuseTextureIndex   = material == nullptr ? 0 : EnsureTextureSlot(material->Material->GetDiffuseTexture());
		uint32_t specularTextureIndex  = material == nullptr ? 0 : EnsureTextureSlot(material->Material->GetSpecularTexture());
		uint32_t normalMapTextureIndex = material == nullptr ? 0 : EnsureTextureSlot(material->Material->GetNormalMap());
		uint32_t heightMapTextureIndex = material == nullptr ? 0 : EnsureTextureSlot(material->Material->GetHeightMap());

		const std::vector<Mesh::Vertex>& vertices = mesh.Mesh->GetVertices();
		const std::vector<uint32_t>& indices = mesh.Mesh->GetIndices();

		if (s_RendererData.MeshVertexCount + vertices.size() >= RendererData::MaxVertices ||
			s_RendererData.MeshIndexCount  + indices .size() >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		for (uint32_t i = 0; i < indices.size(); i++)
		{
			s_RendererData.MeshIndexBufferBase[s_RendererData.MeshIndexCount++] = s_RendererData.MeshVertexCount + indices[i];
		}

		for (size_t i = 0; i < vertices.size(); i++)
		{
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].Position              = transform * glm::vec4(vertices[i].Position, 1.0f);
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].EntityID              = entityID;
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].Normal                = normalMatrix * vertices[i].Normal;
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].TexCoord              = vertices[i].TexCoords;
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].Tangent               = vertices[i].Tangent;

			//s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].Model                 = glm::transpose(glm::inverse(transform));
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].Model                 = transform;

			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].AmbientColor          = material == nullptr ? glm::vec3(1.0f) : material->Material->GetAmbientColor();
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].DiffuseColor          = material == nullptr ? glm::vec3(1.0f) : material->Material->GetDiffuseColor();
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].SpecularColor         = material == nullptr ? glm::vec3(1.0f) : material->Material->GetSpecularColor();
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].Shininess             = material == nullptr ? 0.25f           : material->Material->GetShininess();

			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].DiffuseTextureIndex   = diffuseTextureIndex;
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].SpecularTextureIndex  = specularTextureIndex;
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].NormalMapTextureIndex = normalMapTextureIndex;
			s_RendererData.MeshVertexBufferBase[s_RendererData.MeshVertexCount].HeightMapTextureIndex = heightMapTextureIndex;

			s_RendererData.MeshVertexCount++;
		}

		s_RendererData.Stats.MeshCount++;
		s_RendererData.Stats.TotalVertexCount += vertices.size();
		s_RendererData.Stats.TotalIndexCount  += indices.size();
	}

	void Renderer::DrawMeshOutline(const glm::mat4& transform, const MeshComponent& mesh,const glm::vec4& color, int entityID)
	{
		ATLAS_PROFILE_FUNCTION();

		const std::vector<Mesh::Vertex>& vertices = mesh.Mesh->GetVertices();
		const std::vector<uint32_t>& indices = mesh.Mesh->GetIndices();

		if (s_RendererData.MeshOutlineVertexCount + vertices.size() >= RendererData::MaxVertices ||
			s_RendererData.MeshOutlineIndexCount  + indices .size() >= RendererData::MaxIndices)
		{
			NextBatch();
		}

		for (uint32_t i = 0; i < indices.size(); i++)
		{
			s_RendererData.MeshOutlineIndexBufferBase[s_RendererData.MeshOutlineIndexCount++] = s_RendererData.MeshOutlineVertexCount + indices[i];
		}

		for (size_t i = 0; i < vertices.size(); i++)
		{
			s_RendererData.MeshOutlineVertexBufferBase[s_RendererData.MeshOutlineVertexCount].Position = transform * glm::vec4(vertices[i].Position, 1.0f);
			s_RendererData.MeshOutlineVertexBufferBase[s_RendererData.MeshOutlineVertexCount].Color    = color;
			s_RendererData.MeshOutlineVertexBufferBase[s_RendererData.MeshOutlineVertexCount].EntityID = entityID;

			s_RendererData.MeshOutlineVertexCount++;
		}

		s_RendererData.Stats.SelectionCount++;
	}

	void Renderer::DrawSkybox(const Ref<Cubemap>& skybox)
	{
		// Cubemap
		skybox->Bind();

		// Shader
		s_RendererData.SkyboxShader->Bind();

		// Draw
		RenderCommand::DrawIndexed(s_RendererData.SkyboxVertexArray, s_RendererData.SkyboxIndexCount);
		s_RendererData.Stats.DrawCalls++;
	}
}