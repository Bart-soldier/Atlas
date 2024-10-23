#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLCubemap.h"

#include <stb_image.h>

namespace Atlas
{
	OpenGLCubemap::OpenGLCubemap()
	{
		ATLAS_PROFILE_FUNCTION();

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (int i = 0; i < 6; i++)
		{
			uint32_t blackTextureData = 0x00000000;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &blackTextureData);
		}
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		ATLAS_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubemap::SetFace(const CubemapFace& face, const Ref<Texture2D>& texture)
	{
		ATLAS_PROFILE_FUNCTION();

		if (texture != nullptr)
		{
			if (!LoadFace(face, texture))
			{
				return;
			}

			m_Faces[(int)face] = texture;
		}
		else
		{
			ResetFace(face);
			m_Faces[(int)face] = texture;
			ResetMaxWidth();
		}

		VerifyFaces();
	}

	void OpenGLCubemap::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	bool OpenGLCubemap::LoadFace(const CubemapFace& face, const Ref<Texture2D>& texture)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = nullptr;
		{
			ATLAS_PROFILE_SCOPE("stbi_load - OpenGLCubemap::LoadFace(const CubemapFace& face, const Ref<Texture2D>& texture) ");

			data = stbi_load(texture->GetPath().string().c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			if (m_MaxWidth <= width)
			{
				m_MaxWidth = width;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)face, 0, GL_RGB, m_MaxWidth, m_MaxWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				ATLAS_CORE_WARN("Could not load cubemap face - too small");
			}

			stbi_image_free(data);

			return true;
		}
		else
		{
			ATLAS_CORE_WARN("Could not load cubemap face texture {0}", texture->GetPath().string());
			return false;
		}
	}

	void OpenGLCubemap::ResetFace(const CubemapFace& face)
	{
		std::vector<GLubyte> blackTextureData(m_MaxWidth * m_MaxWidth * 4, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)face, 0, GL_RGB, m_MaxWidth, m_MaxWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, &blackTextureData[0]);
	}

	void OpenGLCubemap::VerifyFaces()
	{
		for (int i = 0; i < 6; i++)
		{
			Ref<Texture2D> face = m_Faces[i];

			if ((face != nullptr && face->GetWidth() < m_MaxWidth) || face == nullptr)
			{
				ResetFace((CubemapFace)i);
				m_Faces[i] = nullptr;
			}
		}
	}

	void OpenGLCubemap::ResetMaxWidth()
	{
		m_MaxWidth = 0;

		for (int i = 0; i < 6; i++)
		{
			uint32_t faceWidth = m_Faces[i] != nullptr ? m_Faces[i]->GetWidth() : 1;

			m_MaxWidth = faceWidth > m_MaxWidth ? faceWidth : m_MaxWidth;
		}
	}
}