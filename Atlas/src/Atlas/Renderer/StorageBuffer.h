#pragma once

#include "Atlas/Core/Base.h"

namespace Atlas
{
	class StorageBuffer
	{
	public:
		virtual ~StorageBuffer() {}
		virtual void SetSize(uint32_t size) = 0;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<StorageBuffer> Create(uint32_t size, uint32_t binding);
		static Ref<StorageBuffer> Create(uint32_t binding);
	};
}