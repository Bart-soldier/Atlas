#include "atlaspch.h"
#include "Platform/Vulkan/VulkanShader.h"

#include <fstream>
#include <vulkan/vulkan.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Atlas/Core/Timer.h"
#include "Atlas/Core/Application.h"

namespace Atlas
{
	namespace Utils
	{
		static ShaderStage ShaderStageFromString(const std::string& type)
		{
			if (type == "vertex")
			{
				return ShaderStage::Vertex;
			}
			if (type == "fragment" || type == "pixel")
			{
				return ShaderStage::Fragment;
			}

			ATLAS_CORE_ASSERT(false, "Unknown shader type!");
			return ShaderStage::None;
		}

		static shaderc_shader_kind ShaderStageToShaderC(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:
				return shaderc_glsl_vertex_shader;
			case ShaderStage::Fragment:
				return shaderc_glsl_fragment_shader;
			}
			ATLAS_CORE_ASSERT(false);
			return (shaderc_shader_kind)0;
		}

		static const char* ShaderStageToString(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:
				return "Vertex";
			case ShaderStage::Fragment:
				return "Fragment";
			}
			ATLAS_CORE_ASSERT(false);
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* ShaderStageCachedOpenGLFileExtension(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:
				return ".cached_opengl.vert";
			case ShaderStage::Fragment:
				return ".cached_opengl.frag";
			}
			ATLAS_CORE_ASSERT(false);
			return "";
		}

		static const char* ShaderStageCachedVulkanFileExtension(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:
				return ".cached_vulkan.vert";
			case ShaderStage::Fragment:
				return ".cached_vulkan.frag";
			}
			ATLAS_CORE_ASSERT(false);
			return "";
		}

		static VkShaderStageFlagBits ShaderStageToVkShaderStageFlagBits(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderStage::Fragment:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			ATLAS_CORE_ASSERT(false);
			return (VkShaderStageFlagBits)0x00000000;
		}
	}

	VulkanShader::VulkanShader(const std::filesystem::path& path)
		: m_VertexPath(path), m_FragmentPath(path)
	{
		ATLAS_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(path);
		auto shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileOrGetVulkanBinaries(shaderSources);
			CreateProgram();
			ATLAS_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		m_Name = path.stem().string();
	}

	VulkanShader::VulkanShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
		: m_VertexPath(vertexPath), m_FragmentPath(fragmentPath)
	{
		ATLAS_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::string vertexSource = ReadFile(vertexPath);
		std::string fragmentSource = ReadFile(fragmentPath);

		std::unordered_map<ShaderStage, std::string> sources;
		sources[ShaderStage::Vertex] = vertexSource;
		sources[ShaderStage::Fragment] = fragmentSource;

		{
			Timer timer;
			CompileOrGetVulkanBinaries(sources);
			CreateProgram();
			ATLAS_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		m_Name = vertexPath.stem().string();
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		ATLAS_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();

		std::unordered_map<ShaderStage, std::string> sources;
		sources[ShaderStage::Vertex] = vertexSrc;
		sources[ShaderStage::Fragment] = fragmentSrc;

		{
			Timer timer;
			CompileOrGetVulkanBinaries(sources);
			CreateProgram();
			ATLAS_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}
	}

	VulkanShader::~VulkanShader()
	{
		ATLAS_PROFILE_FUNCTION();

		//glDeleteProgram(m_RendererID);
	}

	std::string VulkanShader::ReadFile(const std::filesystem::path& path)
	{
		ATLAS_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(path, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				ATLAS_CORE_ERROR("Could not read from file '{0}'", path.string());
			}
		}
		else
		{
			ATLAS_CORE_ERROR("Could not open file '{0}'", path.string());
		}

		return result;
	}

	std::unordered_map<ShaderStage, std::string> VulkanShader::PreProcess(const std::string& source)
	{
		ATLAS_PROFILE_FUNCTION();

		std::unordered_map<ShaderStage, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			ATLAS_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			ATLAS_CORE_ASSERT(Utils::ShaderStageFromString(type) != ShaderStage::None, "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			ATLAS_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::ShaderStageFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void VulkanShader::CompileOrGetVulkanBinaries(const std::unordered_map<ShaderStage, std::string>& shaderSources)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderPath = stage == ShaderStage::Vertex ? m_VertexPath : m_FragmentPath;

			std::filesystem::path cachedPath = cacheDirectory / (shaderPath.filename().string() + Utils::ShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::ShaderStageToShaderC(stage), shaderPath.string().c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ATLAS_CORE_ERROR(module.GetErrorMessage());
					ATLAS_CORE_ASSERT(false);
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void VulkanShader::CreateProgram()
	{
		//GLuint program = glCreateProgram();

		Application& app = Application::Get();
		VkDevice device = (VkDevice)app.GetGraphicsContext()->GetLogicalDevice();

		std::vector<VkShaderModule> shaderModules;
		shaderModules.reserve(m_VulkanSPIRV.size());
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = spirv.size() * sizeof(uint32_t);
			createInfo.pCode = spirv.data();

			VkShaderModule shaderModule;
			VkResult status = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
			ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create shader module!");

			shaderModules.emplace_back(shaderModule);

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = Utils::ShaderStageToVkShaderStageFlagBits(stage);
			fragShaderStageInfo.module = shaderModule;
			fragShaderStageInfo.pName = "main";
		}





		for (VkShaderModule shaderModule : shaderModules)
		{
			vkDestroyShaderModule(device, shaderModule, nullptr);
		}

		//glLinkProgram(program);

		//GLint isLinked;
		//glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		//if (isLinked == GL_FALSE)
		//{
		//	GLint maxLength;
		//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
		//	ATLAS_CORE_ERROR("Shader linking failed ({0}, {1}):\n{2}", m_VertexPath.string(), m_FragmentPath.string(), infoLog.data());

		//	glDeleteProgram(program);

		//	for (auto id : shaderIDs)
		//	{
		//		glDeleteShader(id);
		//	}
		//}

		//for (auto id : shaderIDs)
		//{
		//	glDetachShader(program, id);
		//	glDeleteShader(id);
		//}

		//m_RendererID = program;
	}

	void VulkanShader::Reflect(ShaderStage stage, const std::vector<uint32_t>& shaderData)
	{
		std::filesystem::path shaderPath = stage == ShaderStage::Vertex ? m_VertexPath : m_FragmentPath;

		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		ATLAS_CORE_TRACE("VulkanShader::Reflect - {0} - {1}", Utils::ShaderStageToString(stage), shaderPath.string());
		ATLAS_CORE_TRACE("    {0} inputs", resources.stage_inputs.size());
		ATLAS_CORE_TRACE("    {0} ouputs", resources.stage_outputs.size());
		ATLAS_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		ATLAS_CORE_TRACE("    {0} storage buffers", resources.storage_buffers.size());
		ATLAS_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		if (resources.stage_inputs.size() > 0)
		{
			ATLAS_CORE_TRACE("Inputs:");
			for (const auto& resource : resources.stage_inputs)
			{
				uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
				std::string name = compiler.get_fallback_name(resource.id);

				ATLAS_CORE_TRACE("  {0}", name);
				ATLAS_CORE_TRACE("    Location = {0}", location);
			}
		}

		if (resources.stage_outputs.size() > 0)
		{
			ATLAS_CORE_TRACE("Outputs:");
			for (const auto& resource : resources.stage_outputs)
			{
				uint32_t location = compiler.get_decoration(resource.id, spv::DecorationLocation);
				std::string name = compiler.get_fallback_name(resource.id);

				ATLAS_CORE_TRACE("  {0}", name);
				ATLAS_CORE_TRACE("    Location = {0}", location);
			}
		}

		if (resources.uniform_buffers.size() > 0)
		{
			ATLAS_CORE_TRACE("Uniform buffers:");
			for (const auto& resource : resources.uniform_buffers)
			{
				const auto& bufferType = compiler.get_type(resource.base_type_id);
				uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				int memberCount = bufferType.member_types.size();

				ATLAS_CORE_TRACE("  {0}", resource.name);
				ATLAS_CORE_TRACE("    Size = {0}", bufferSize);
				ATLAS_CORE_TRACE("    Binding = {0}", binding);
				ATLAS_CORE_TRACE("    Members = {0}", memberCount);
			}
		}

		if (resources.storage_buffers.size() > 0)
		{
			ATLAS_CORE_TRACE("Storage buffers:");
			for (const auto& resource : resources.storage_buffers)
			{
				const auto& bufferType = compiler.get_type(resource.base_type_id);
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				int memberCount = bufferType.member_types.size();

				ATLAS_CORE_TRACE("  {0}", resource.name);
				ATLAS_CORE_TRACE("    Binding = {0}", binding);
				ATLAS_CORE_TRACE("    Members = {0}", memberCount);
			}
		}
	}

	void VulkanShader::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glUseProgram(m_RendererID);
	}

	void VulkanShader::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glUseProgram(0);
	}

	void VulkanShader::SetInt(const std::string& name, int value)
	{
		ATLAS_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void VulkanShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void VulkanShader::SetFloat(const std::string& name, float value)
	{
		ATLAS_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void VulkanShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		ATLAS_PROFILE_FUNCTION();

		UploadUniformFloat2(name, value);
	}

	void VulkanShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		ATLAS_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void VulkanShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		ATLAS_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void VulkanShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		ATLAS_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void VulkanShader::UploadUniformInt(const std::string& name, int value)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform1i(location, value);
	}

	void VulkanShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform1iv(location, count, values);
	}

	void VulkanShader::UploadUniformFloat(const std::string& name, float value)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform1f(location, value);
	}

	void VulkanShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform2f(location, value.x, value.y);
	}

	void VulkanShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform3f(location, value.x, value.y, value.z);
	}

	void VulkanShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void VulkanShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void VulkanShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		//GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		//glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}