#pragma once

#include "Atlas/Renderer/StorageBuffer.h"

namespace Atlas
{
	class OpenGLStorageBuffer : public StorageBuffer
	{
	public:
		OpenGLStorageBuffer(uint32_t size, uint32_t binding);
		OpenGLStorageBuffer(uint32_t binding);
		virtual ~OpenGLStorageBuffer();

		virtual void SetSize(uint32_t size) override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Binding = 0;
	};
}