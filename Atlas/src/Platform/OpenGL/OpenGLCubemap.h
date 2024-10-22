#pragma once

#include "Atlas/Renderer/Cubemap.h"

#include <glad/glad.h>

namespace Atlas
{
	class OpenGLCubemap : public Cubemap
	{
	public:
		OpenGLCubemap();
		virtual ~OpenGLCubemap();

		virtual uint32_t GetRendererID() const override { return m_RendererID; };

		virtual void SetFace(const CubemapFace& face, const Ref<Texture2D>& texture) override;
		virtual const Ref<Texture2D>& GetFace(const CubemapFace& face) override { return m_Faces[(int)face]; };

		virtual void Bind() const override;

		virtual bool operator==(const Cubemap& other) const override
		{
			return m_RendererID == other.GetRendererID();
		};

	private:
		bool LoadFace(const CubemapFace& face, const Ref<Texture2D>& texture);
		void ResetFace(const CubemapFace& face);
		void VerifyFaces();
		void ResetMaxWidth();


		Ref<Texture2D> m_Faces[6];
		uint32_t m_RendererID;
		uint32_t m_MaxWidth = 1;
	};
}