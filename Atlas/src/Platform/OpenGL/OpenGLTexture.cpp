#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace Atlas
{
	namespace Utils
	{
		static GLenum AtlasImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB;
			case ImageFormat::RGBA8: return GL_RGBA;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum AtlasImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification)
	{
		CreateTextureStorage(m_Specification);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& path, const bool generateMips)
		: m_Path(path)
	{
		ATLAS_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			ATLAS_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path&) ");

			data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			m_IsLoaded = true;

			m_Specification = TextureSpecification();
			m_Specification.Width = width;
			m_Specification.Height = height;
			m_Specification.GenerateMips = generateMips;

			if (channels == 4)
			{
				m_Specification.Format = ImageFormat::RGBA8;
			}
			else if (channels == 3)
			{
				m_Specification.Format = ImageFormat::RGB8;
			}
			else
			{
				m_Specification.Format = ImageFormat::None;
				ATLAS_CORE_ASSERT(false, "Format not supported!");
			}

			CreateTextureStorage(m_Specification);

			SetData(data, m_Specification.Width * m_Specification.Height * (m_DataFormat == GL_RGBA ? 4 : 3));

			stbi_image_free(data);
		}
		else
		{
			ATLAS_CORE_WARN("Could not load texture {0}", path.string());
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		ATLAS_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		ATLAS_CORE_ASSERT(size == m_Specification.Width * m_Specification.Height * (m_DataFormat == GL_RGBA ? 4 : 3), "Data must be entire texture!");

		m_IsLoaded = true;
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		if (m_Specification.GenerateMips)
		{
			glGenerateTextureMipmap(m_RendererID);
		}
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::CreateTextureStorage(const TextureSpecification& specification)
	{
		ATLAS_PROFILE_FUNCTION();

		m_InternalFormat = Utils::AtlasImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = Utils::AtlasImageFormatToGLDataFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Specification.Width, m_Specification.Height);

		GLenum minFilter = GL_LINEAR;
		if (m_Specification.GenerateMips)
		{
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}