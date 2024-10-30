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
			case ImageFormat::R8:      return GL_RED;

			case ImageFormat::RG16F:   return GL_RG;

			case ImageFormat::RGB16F:
			case ImageFormat::RGB32F:
			case ImageFormat::RGB8:    return GL_RGB;

			case ImageFormat::RGBA8:
			case ImageFormat::RGBA16F:
			case ImageFormat::RGBA32F: return GL_RGBA;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum AtlasImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::R8:      return GL_R8;

			case ImageFormat::RG16F:   return GL_RG16F;

			case ImageFormat::RGB8:    return GL_RGB8;
			case ImageFormat::RGB16F:  return GL_RGB16F;
			case ImageFormat::RGB32F:  return GL_RGB32F;

			case ImageFormat::RGBA8:   return GL_RGBA8;
			case ImageFormat::RGBA16F: return GL_RGBA16F;
			case ImageFormat::RGBA32F: return GL_RGBA32F;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLint AtlasResizeFilterToGLInt(ResizeFilter filter)
		{
			switch (filter)
			{
			case Atlas::ResizeFilter::Linear:  return GL_LINEAR;
			case Atlas::ResizeFilter::Nearest: return GL_NEAREST;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLint AtlasWrapToGLInt(Wrap wrap)
		{
			switch (wrap)
			{
			case Atlas::Wrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
			case Atlas::Wrap::Repeat:      return GL_REPEAT;
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

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& path, const bool generateMips, const bool flipOnLoad)
		: m_Path(path)
	{
		ATLAS_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(flipOnLoad);
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
				if (path.extension() == ".hdr")
				{
					m_Specification.Format = ImageFormat::RGB16F;
				}
				else
				{
					m_Specification.Format = ImageFormat::RGB8;
				}
			}
			else if (channels == 1)
			{
				m_Specification.Format = ImageFormat::R8;
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

		//ATLAS_CORE_ASSERT(size == m_Specification.Width * m_Specification.Height *
		//	(Utils::GLInternalFormatToBytesPerChannel(m_InternalFormat)) * (m_DataFormat == GL_RGBA ? 4 : 3), "Data must be entire texture!");

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

		GLenum minFilter = Utils::AtlasResizeFilterToGLInt(m_Specification.MinFilter);
		if (m_Specification.GenerateMips)
		{
			switch (minFilter)
			{
			case GL_LINEAR:  minFilter = GL_LINEAR_MIPMAP_LINEAR;  break;
			case GL_NEAREST: minFilter = GL_NEAREST_MIPMAP_LINEAR; break;
			}
		}

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, Utils::AtlasResizeFilterToGLInt(m_Specification.MagFilter));

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, Utils::AtlasWrapToGLInt(m_Specification.WrapS));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, Utils::AtlasWrapToGLInt(m_Specification.WrapT));
	}
}