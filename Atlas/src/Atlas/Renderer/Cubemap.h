#pragma once

#include "Atlas/Renderer/Texture.h"

namespace Atlas
{
	enum class CubemapFace
	{
		Right  = 0,
		Left   = 1,
		Top    = 2,
		Bottom = 3,
		Back   = 4,
		Front  = 5
	};

	class Cubemap
	{
	public:
		virtual ~Cubemap() = default;

		virtual uint32_t GetRendererID() const = 0;

		virtual void SetFace(const CubemapFace& face, const Ref<Texture2D>& texture) = 0;
		virtual const Ref<Texture2D>& GetFace(const CubemapFace& face) = 0;

		virtual void Bind() const = 0;

		virtual bool operator==(const Cubemap& other) const = 0;

		static Ref<Cubemap> Create();
	};
}
