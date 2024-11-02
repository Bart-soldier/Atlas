#include "atlaspch.h"
#include "Atlas/Renderer/Shader.h"

#include "Atlas/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Atlas
{
	Ref<Shader> Shader::Create(const std::filesystem::path& path)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(path);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanShader>(path);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(vertexPath, fragmentPath);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanShader>(vertexPath, fragmentPath);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanShader>(name, vertexSrc, fragmentSrc);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		ATLAS_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::filesystem::path& path)
	{
		auto shader = Shader::Create(path);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::filesystem::path& path)
	{
		auto shader = Shader::Create(path);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		ATLAS_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}