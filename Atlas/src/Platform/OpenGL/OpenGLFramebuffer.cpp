#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Atlas
{
	static const uint32_t s_MaxFrameBufferSize = 8192;

	namespace Utils
	{
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24_STENCIL8:  return true;
			case FramebufferTextureFormat::DEPTH32F:          return true;
			}

			return false;
		}

		static GLenum AtlasFBTextureFormatToGLAttachementType(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24_STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;
			case FramebufferTextureFormat::DEPTH32F:         return GL_DEPTH_ATTACHMENT;
			}

			ATLAS_CORE_ASSERT(false, "Format not supported for depth/stencil framebuffers!");
			return 0;
		}

		static GLenum GLInternalFormatToGLType(GLenum format)
		{
			switch (format)
			{
			case GL_RGBA8:
			case GL_RGBA16:
			case GL_R32I:    return GL_UNSIGNED_BYTE;
			case GL_R32F:
			case GL_RGBA16F: return GL_FLOAT;
			}

			ATLAS_CORE_ASSERT(false, "Format not supported for color framebuffers!");
			return 0;
		}

		static GLenum AtlasFBTextureFormatToGLDataFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:
			case FramebufferTextureFormat::RGBA16:
			case FramebufferTextureFormat::RGBA16F:     return GL_RGBA;
			case FramebufferTextureFormat::RED32I:      return GL_RED_INTEGER;
			case FramebufferTextureFormat::RED32F:      return GL_RED;
			}

			ATLAS_CORE_ASSERT(false, "Format not supported for color framebuffers!");
			return 0;
		}

		static GLenum AtlasFBTextureFormatToGLInternalFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:            return GL_RGBA8;
			case FramebufferTextureFormat::RGBA16:           return GL_RGBA16;
			case FramebufferTextureFormat::RGBA16F:          return GL_RGBA16F;
			case FramebufferTextureFormat::RED32I:           return GL_R32I;
			case FramebufferTextureFormat::RED32F:           return GL_R32F;
			case FramebufferTextureFormat::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
			case FramebufferTextureFormat::DEPTH32F:         return GL_DEPTH_COMPONENT32F;
			}

			ATLAS_CORE_ASSERT(false, "Format not supported for framebuffers!");
			return 0;
		}

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GLInternalFormatToGLType(internalFormat), nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Reset();
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			Reset();
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples,
					Utils::AtlasFBTextureFormatToGLInternalFormat(m_ColorAttachmentSpecifications[i].TextureFormat),
					Utils::AtlasFBTextureFormatToGLDataFormat(m_ColorAttachmentSpecifications[i].TextureFormat),
					m_Specification.Width, m_Specification.Height, i);
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);

			Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples,
				Utils::AtlasFBTextureFormatToGLInternalFormat(m_DepthAttachmentSpecification.TextureFormat),
				Utils::AtlasFBTextureFormatToGLAttachementType(m_DepthAttachmentSpecification.TextureFormat),
				m_Specification.Width, m_Specification.Height);
		}

		EnableAllColorAttachments();

		ATLAS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			ATLAS_CORE_WARN("Attempted to resize frame buffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		ATLAS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData = 0;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFramebuffer::CopyColor(Ref<Framebuffer> framebuffer)
	{
		glBlitNamedFramebuffer(framebuffer->GetRendererID(), GetRendererID(),
			0, 0, m_Specification.Width, m_Specification.Height, 0, 0, framebuffer->GetSpecification().Width, framebuffer->GetSpecification().Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void OpenGLFramebuffer::CopyDepth(Ref<Framebuffer> framebuffer)
	{
		glBlitNamedFramebuffer(framebuffer->GetRendererID(), GetRendererID(),
			0, 0, m_Specification.Width, m_Specification.Height, 0, 0, framebuffer->GetSpecification().Width, framebuffer->GetSpecification().Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		ATLAS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			Utils::AtlasFBTextureFormatToGLDataFormat(spec.TextureFormat), GL_INT, &value);
	}

	void OpenGLFramebuffer::EnableColorAttachment(uint32_t attachmentIndex)
	{
		ATLAS_CORE_ASSERT(m_ColorAttachments.size() >= 1);

		GLenum buffer = GL_COLOR_ATTACHMENT0 + attachmentIndex;
		glDrawBuffer(buffer);
	}

	void OpenGLFramebuffer::EnableColorAttachments(std::initializer_list<uint32_t> attachmentIndices)
	{
		ATLAS_CORE_ASSERT(attachmentIndices.size() <= m_ColorAttachments.size());

		if (attachmentIndices.size() >= 1)
		{
			ATLAS_CORE_ASSERT(m_ColorAttachments.size() <= 8);

			std::vector<GLenum> buffers;
			buffers.reserve(8);

			for (uint32_t index : attachmentIndices)
			{
				ATLAS_CORE_ASSERT(index < 8);
				buffers.push_back(GL_COLOR_ATTACHMENT0 + index);
			}

			glDrawBuffers(buffers.size(), buffers.data());
		}
		else
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}
	}

	void OpenGLFramebuffer::EnableAllColorAttachments()
	{
		if (m_ColorAttachments.size() > 1)
		{
			ATLAS_CORE_ASSERT(m_ColorAttachments.size() <= 8);
			GLenum buffers[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
								  GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}
	}

	void OpenGLFramebuffer::Reset()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
	}
}