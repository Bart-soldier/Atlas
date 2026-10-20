#pragma once

#include "Atlas/Renderer/Framebuffer.h"
#include <vma/vk_mem_alloc.h>

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
		VkImage GetColorImage(uint32_t index = 0) const { ATLAS_CORE_ASSERT(index < m_ColorImages.size()); return m_ColorImages[index]; }
		VkImageView GetColorImageView(uint32_t index = 0) const { ATLAS_CORE_ASSERT(index < m_ColorImageViews.size()); return m_ColorImageViews[index]; }
		VkFormat GetColorFormat(uint32_t index = 0) const { ATLAS_CORE_ASSERT(index < m_ColorFormats.size()); return m_ColorFormats[index]; }
		const std::vector<uint32_t>& GetActiveColorAttachments() const { return m_ActiveColorAttachments; }
		VkImageView GetDepthImageView() const { return m_DepthImageView; }
		VkImage GetDepthImage() const { return m_DepthImage; }
		VkImageLayout GetDepthImageLayout() const { return m_DepthImageLayout; }
		void SetDepthImageLayout(VkImageLayout layout) { m_DepthImageLayout = layout; }
		VkSampler GetColorSampler() const { return m_ColorSampler; }
		VkImageLayout GetColorImageLayout() const { return m_ColorImageLayout; }
		void SetColorImageLayout(VkImageLayout layout);
		static VulkanFramebuffer* GetActive() { return s_Active; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		void Reset();
		void CreateAttachment(VkFormat format, VkImageUsageFlags usage, VkImage& image, VkImageView& view, VmaAllocation& allocation);

		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		std::vector<VkImage> m_ColorImages;
		std::vector<VkImageView> m_ColorImageViews;
		std::vector<VmaAllocation> m_ColorAllocations;
		std::vector<VkFormat> m_ColorFormats;
		std::vector<uint32_t> m_ActiveColorAttachments;
		VkImage m_DepthImage = VK_NULL_HANDLE;
		VkImageView m_DepthImageView = VK_NULL_HANDLE;
		VmaAllocation m_DepthAllocation = VK_NULL_HANDLE;
		VkSampler m_ColorSampler = VK_NULL_HANDLE;
		VkImageLayout m_ColorImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout m_DepthImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		inline static VulkanFramebuffer* s_Active = nullptr;
	};
}