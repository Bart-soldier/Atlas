#pragma once

#include "Atlas/Renderer/StorageBuffer.h"

namespace Atlas
{
	class VulkanStorageBuffer : public StorageBuffer
	{
	public:
		VulkanStorageBuffer(uint32_t size, uint32_t binding);
		VulkanStorageBuffer(uint32_t binding);
		virtual ~VulkanStorageBuffer();

		virtual void SetSize(uint32_t size) override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Binding = 0;
	};
}