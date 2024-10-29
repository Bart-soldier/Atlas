#pragma once

#include "Atlas/Renderer/Texture.h"

namespace Atlas
{
	class Cubemap
	{
	public:
		virtual ~Cubemap() = default;

		virtual uint32_t GetRendererID() const = 0;

		virtual void SetMap(const Ref<Texture2D>& texture) = 0;
		virtual const Ref<Texture2D>& GetMap() = 0;

		virtual void Bind() const = 0;

		virtual bool operator==(const Cubemap& other) const = 0;

		static Ref<Cubemap> Create();
	};
}
