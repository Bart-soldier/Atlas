#pragma once

#include "Atlas/Renderer/Framebuffer.h"

namespace Atlas
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void CopyColor(Ref<Framebuffer> framebuffer) override;
		virtual void CopyDepth(Ref<Framebuffer> framebuffer) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual void EnableColorAttachment(uint32_t attachmentIndex) override;
		virtual void EnableColorAttachments(std::initializer_list<uint32_t> attachmentIndices) override;
		virtual void EnableAllColorAttachments() override;

		virtual uint32_t GetRendererID() const override { ATLAS_CORE_ASSERT(m_RendererID != 0); return m_RendererID; };
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { ATLAS_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }
		virtual uint32_t GetDepthAttachmentRendererID() const override { ATLAS_CORE_ASSERT(m_DepthAttachment != 0); return m_DepthAttachment; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		void Reset();

		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}