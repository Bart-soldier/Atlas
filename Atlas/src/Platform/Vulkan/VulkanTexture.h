#pragma once

#include "Atlas/Renderer/Texture.h"
#include <vma/vk_mem_alloc.h>

namespace Atlas
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const TextureSpecification& specification);
		VulkanTexture2D(const std::filesystem::path& path, const bool generateMips = true, const bool flipOnLoad = true);
		virtual ~VulkanTexture2D();

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual const std::filesystem::path& GetPath() const override { return m_Path; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;
		VkImageView GetImageView() const { return m_ImageView; }
		VkSampler GetSampler() const { return m_Sampler; }

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		};

	private:
		void CreateTextureStorage(const TextureSpecification& specification);
		void Upload(void* data, uint32_t size);

		TextureSpecification m_Specification;

		std::filesystem::path m_Path;
		bool m_IsLoaded = false;
		uint32_t m_RendererID = 0;
		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;
	};
}
