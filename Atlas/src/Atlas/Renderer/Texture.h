#pragma once

namespace Atlas
{
	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGB16F,
		RGBA16F,
		RGB32F,
		RGBA32F
	};

	enum class ResizeFilter
	{
		None = 0,
		Linear,
		Nearest
	};

	enum class Wrap
	{
		None = 0,
		ClampToEdge,
		Repeat
	};

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;

		ResizeFilter MagFilter = ResizeFilter::Linear;
		ResizeFilter MinFilter = ResizeFilter::Linear;
		Wrap WrapS = Wrap::ClampToEdge;
		Wrap WrapT = Wrap::ClampToEdge;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& specification);
		static Ref<Texture2D> Create(const std::filesystem::path& path, const bool generateMips = true);
	};
}
