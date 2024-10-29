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

		virtual uint32_t GetRendererID() const override { return m_RendererID; };

		virtual void SetMap(const Ref<Texture2D>& cubemap) override;
		virtual const Ref<Texture2D>& GetMap() override { return m_Map; };

		virtual void Bind() const override;

		virtual bool operator==(const Cubemap& other) const override
		{
			return m_RendererID == other.GetRendererID();
		};

	private:
		void ResetMap();
		void LoadMap();

		Ref<Texture2D> m_Map;
		uint32_t m_RendererID;
		Ref<Shader> m_EquirectangularToCubemapShader;
	};
}