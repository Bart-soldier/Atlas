#pragma once

#include "Atlas/Renderer/RenderCommand.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture2D.h"
#include "Atlas/Renderer/Cubemap.h"
#include "Atlas/Renderer/Camera.h"
#include "Atlas/Renderer/EditorCamera.h"

#include "Atlas/Scene/Components.h"

namespace Atlas
{
	class Renderer
	{
	public:
		struct LightData
		{
			glm::vec4 Position;
			glm::vec4 Color;
			glm::vec4 Direction; // w is a flag to indicate if light direction is spot direction
			float Radius;
			float Intensity;
			glm::vec2 CutOffs; // (inner, outer); negative value means cutoff is disabled
			float AmbientStrength;
			float DiffuseStrength;
			float SpecularStrength;
			float Padding; // DO NOT USE: For shader alignment
		};

		static void Init();
		static void Shutdown();

		static void EnableStencilWriting();
		static void DisableStencilWriting();

		static uint32_t GetLightStorageBufferCapacity();

		static RendererAPI::PolygonMode GetPolygonMode();
		static void SetPolygonMode(RendererAPI::PolygonMode polygonMode);

		// Settings
		static const float& GetGamma();
		static void SetGamma(float gamma);
		static const float& GetExposure();
		static void SetExposure(float exposure);
		static const float& GetParallaxScale();
		static void SetParallaxScale(float scale);
		static const float& GetBloomThreshold();
		static void SetBloomThreshold(float threshold);
		static bool IsFlatShaderEnabled();
		static void ToggleFlatShader();
		static bool IsHDREnabled();
		static void ToggleHDR();
		static bool IsBloomEnabled();
		static void ToggleBloom();

		static void BeginRenderPass();
		static void EndRenderPass();
		static bool ResizeFramebuffer(uint32_t width, uint32_t height);
		static uint32_t GetDefaultRenderID();
		static uint32_t GetLastFramebufferRenderID();
		static uint32_t GetBrightnessFramebufferRenderID();
		static int GetEntityIDFromPixel(int x, int y);

		static void BeginScene(const Camera& camera, const TransformComponent& cameraTransform, const std::vector<LightData>& lights);
		static void BeginScene(const EditorCamera& camera, const std::vector<LightData>& lights);
		static void EndScene();
		static void NextBatch();

		static void BeginPostProcessing();
		static void EndPostProcessing();
		static void DrawPostProcessing(PostProcessorComponent* postProcessor);

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);
		
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, int entityID);
		
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void DrawMesh(const glm::mat4& transform, const MeshComponent& mesh, const MaterialComponent* material, int entityID);
		static void DrawMeshOutline(const glm::mat4& transform, const MeshComponent& mesh, const glm::vec4& color, int entityID);

		static void DrawSkybox(const Ref<Cubemap>& skybox);

		// Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;
			uint32_t MeshCount = 0;
			uint32_t SelectionCount = 0;
			uint32_t TotalVertexCount = 0;
			uint32_t TotalIndexCount = 0;
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void InitArrays();
		static void InitSkybox();
		static void InitTexture();
		static void InitShaders();
		static void InitBuffers();

		static void SetUniformBuffers(const glm::mat4& cameraProjection, const glm::mat4& cameraView, const glm::vec4& cameraPosition);
		static void SetStorageBuffers(const std::vector<LightData>& lights);
		static void EnsureLightStorageBufferCapacity(uint32_t capacity);
		static uint32_t EnsureTextureSlot(const Ref<Texture2D>& texture);

		static void StartBatch();
		static void Flush();

		static void TogglePostProcessingFramebuffers();
		static void ApplyBloom();
	};
}
