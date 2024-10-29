#pragma once

#include "Atlas/Renderer/Cubemap.h"
#include "Atlas/Renderer/Shader.h"

#include <glad/glad.h>

namespace Atlas
{
	class OpenGLCubemap : public Cubemap
	{
	public:
		OpenGLCubemap();
		virtual ~OpenGLCubemap();

		virtual uint32_t GetRendererID() const override { return m_CubemapRendererID; };

		virtual void SetMap(const Ref<Texture2D>& cubemap) override;
		virtual const Ref<Texture2D>& GetMap() override { return m_Map; };

		virtual void BindCubemap() const override;
		virtual void BindIrradianceMap() const override;

		virtual bool operator==(const Cubemap& other) const override
		{
			return m_CubemapRendererID == other.GetRendererID();
		};

	private:
		void CreateMap(uint32_t* rendererID);
		void ResetMap(uint32_t* rendererID);
		void LoadCubemap();
		void LoadIrradianceMap();

		Ref<Texture2D> m_Map;
		Ref<Shader> m_MapToCubemapShader;
		Ref<Shader> m_CubemapToIrradianceShader;
		uint32_t m_CubemapRendererID;
		uint32_t m_IrradianceRendererID;

		glm::mat4 m_CaptureProjection;
		glm::mat4 m_CaptureViews[6];
	};
}