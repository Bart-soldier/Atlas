#pragma once

#include "Atlas/Renderer/Texture.h"

namespace Atlas
{
	class Cubemap
	{
	public:
		enum class MapType
		{
			None = 0,
			Cubemap = 1,
			IrradianceMap = 2,
			PreFilteredMap = 3,
		};

		virtual ~Cubemap() = default;

		virtual uint32_t GetRendererID() const = 0;

		virtual void SetMap(const Ref<Texture2D>& texture) = 0;
		virtual const Ref<Texture2D>& GetMap() = 0;

		virtual void BindCubemap(uint32_t slot = 0) const = 0;
		virtual void BindIrradianceMap(uint32_t slot = 0) const = 0;
		virtual void BindPreFilteredMap(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Cubemap& other) const = 0;

		static Ref<Cubemap> Create();
	};
}
