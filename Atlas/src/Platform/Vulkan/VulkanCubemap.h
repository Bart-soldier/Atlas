#pragma once

#include "Atlas/Renderer/Cubemap.h"
#include "Atlas/Renderer/Shader.h"

namespace Atlas
{
	class VulkanCubemap : public Cubemap
	{
	public:
		VulkanCubemap();
		virtual ~VulkanCubemap();

		virtual uint32_t GetRendererID() const override { return m_CubemapRendererID; };

		virtual void SetMap(const Ref<Texture2D>& cubemap) override;
		virtual const Ref<Texture2D>& GetMap() override { return m_Map; };

		virtual void BindCubemap(uint32_t slot = 0) const override;
		virtual void BindIrradianceMap(uint32_t slot = 0) const override;
		virtual void BindPreFilteredMap(uint32_t slot = 0) const override;

		virtual bool operator==(const Cubemap& other) const override
		{
			return m_CubemapRendererID == other.GetRendererID();
		};

	private:
		void CreateMap(uint32_t* rendererID, bool generateMips = false);
		void ResetMap(uint32_t* rendererID, bool generateMips = false);
		void LoadCubemap();
		void LoadIrradianceMap();
		void LoadPreFilteredMap();

		Ref<Texture2D> m_Map;
		Ref<Shader> m_MapToCubemapShader;
		Ref<Shader> m_CubemapToIrradianceShader;
		Ref<Shader> m_CubemapToPreFilteredShader;
		uint32_t m_CubemapRendererID;
		uint32_t m_IrradianceRendererID;
		uint32_t m_PreFilteredRendererID;

		glm::mat4 m_CaptureProjection;
		glm::mat4 m_CaptureViews[6];
	};
}