#include "atlaspch.h"
#include "Platform/Vulkan/VulkanCubemap.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Renderer/UniformBuffer.h"
#include "Atlas/Renderer/Framebuffer.h"

#include <stb_image.h>

namespace Atlas
{
	VulkanCubemap::VulkanCubemap()
	{
		ATLAS_PROFILE_FUNCTION();

		//m_MapToCubemapShader = Shader::Create("assets/shaders/Cubemap/Cubemap_Vert.glsl", "assets/shaders/Cubemap/MapToCubemap_Frag.glsl");
		//m_CubemapToIrradianceShader = Shader::Create("assets/shaders/Cubemap/Cubemap_Vert.glsl", "assets/shaders/Cubemap/CubemapToIrradiance_Frag.glsl");
		//m_CubemapToPreFilteredShader = Shader::Create("assets/shaders/Cubemap/Cubemap_Vert.glsl", "assets/shaders/Cubemap/CubemapToPreFiltered_Frag.glsl");

		CreateMap(&m_CubemapRendererID, true);
		CreateMap(&m_IrradianceRendererID);
		CreateMap(&m_PreFilteredRendererID, true);

		m_CaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		m_CaptureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_CaptureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_CaptureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_CaptureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		m_CaptureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		m_CaptureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	}

	void VulkanCubemap::CreateMap(uint32_t* rendererID, bool generateMips)
	{
		//glGenTextures(1, rendererID);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, *rendererID);

		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, generateMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		ResetMap(rendererID, generateMips);
	}

	void VulkanCubemap::ResetMap(uint32_t* rendererID, bool generateMips)
	{
		//glBindTexture(GL_TEXTURE_CUBE_MAP, *rendererID);

		//for (int i = 0; i < 6; i++)
		//{
		//	uint32_t blackTextureData = 0x00000000;
		//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &blackTextureData);
		//}

		//if (generateMips)
		//{
		//	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		//}
	}

	VulkanCubemap::~VulkanCubemap()
	{
		ATLAS_PROFILE_FUNCTION();

		//glDeleteTextures(1, &m_CubemapRendererID);
	}

	void VulkanCubemap::SetMap(const Ref<Texture2D>& cubemap)
	{
		ATLAS_PROFILE_FUNCTION();

		if (m_Map == cubemap)
		{
			return;
		}

		m_Map = cubemap;

		if (m_Map != nullptr)
		{
			LoadCubemap();
			LoadIrradianceMap();
			LoadPreFilteredMap();
		}
		else
		{
			ResetMap(&m_CubemapRendererID);
			ResetMap(&m_IrradianceRendererID);
			ResetMap(&m_PreFilteredRendererID, true);
		}
	}

	void VulkanCubemap::BindCubemap(uint32_t slot) const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindTextureUnit(slot, m_CubemapRendererID);
	}

	void VulkanCubemap::BindIrradianceMap(uint32_t slot) const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindTextureUnit(slot, m_IrradianceRendererID);
	}

	void VulkanCubemap::BindPreFilteredMap(uint32_t slot) const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindTextureUnit(slot, m_PreFilteredRendererID);
	}

	void VulkanCubemap::LoadCubemap()
	{
		BindCubemap();
		for (unsigned int i = 0; i < 6; ++i)
		{
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		m_MapToCubemapShader->Bind();
		m_Map->Bind();

		std::vector<glm::mat4> viewProjectionData;
		viewProjectionData.reserve(2);
		viewProjectionData.push_back(m_CaptureProjection);
		viewProjectionData.push_back(m_CaptureViews[0]);

		Ref<UniformBuffer> viewProjectionUniformBuffer;
		viewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * viewProjectionData.size(), 4);

		unsigned int captureFBO, captureRBO;
		//glGenFramebuffers(1, &captureFBO);
		//glGenRenderbuffers(1, &captureRBO);

		//glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		//glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		//glViewport(0, 0, 512, 512);

		for (unsigned int i = 0; i < 6; ++i)
		{
			viewProjectionData.at(1) = m_CaptureViews[i];
			viewProjectionUniformBuffer->SetData(viewProjectionData.data(), sizeof(glm::mat4) * viewProjectionData.size());

			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			//	GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_CubemapRendererID, 0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::DrawCube();
		}

		//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDeleteFramebuffers(1, &captureFBO);
		//glDeleteRenderbuffers(1, &captureRBO);
		viewProjectionUniformBuffer = nullptr;
	}

	void VulkanCubemap::LoadIrradianceMap()
	{
		BindIrradianceMap();
		for (unsigned int i = 0; i < 6; ++i)
		{
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		m_CubemapToIrradianceShader->Bind();
		BindCubemap();

		std::vector<glm::mat4> viewProjectionData;
		viewProjectionData.reserve(2);
		viewProjectionData.push_back(m_CaptureProjection);
		viewProjectionData.push_back(m_CaptureViews[0]);

		Ref<UniformBuffer> viewProjectionUniformBuffer;
		viewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * viewProjectionData.size(), 4);

		unsigned int captureFBO, captureRBO;
		//glGenFramebuffers(1, &captureFBO);
		//glGenRenderbuffers(1, &captureRBO);

		//glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		//glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		//glViewport(0, 0, 32, 32);

		for (unsigned int i = 0; i < 6; ++i)
		{
			viewProjectionData.at(1) = m_CaptureViews[i];
			viewProjectionUniformBuffer->SetData(viewProjectionData.data(), sizeof(glm::mat4) * viewProjectionData.size());

			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			//	GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceRendererID, 0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::DrawCube();
		}

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDeleteFramebuffers(1, &captureFBO);
		//glDeleteRenderbuffers(1, &captureRBO);
		viewProjectionUniformBuffer = nullptr;
	}

	void VulkanCubemap::LoadPreFilteredMap()
	{
		BindPreFilteredMap();
		for (unsigned int i = 0; i < 6; ++i)
		{
			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		m_CubemapToPreFilteredShader->Bind();
		BindCubemap();

		std::vector<glm::mat4> viewProjectionData;
		viewProjectionData.reserve(2);
		viewProjectionData.push_back(m_CaptureProjection);
		viewProjectionData.push_back(m_CaptureViews[0]);

		Ref<UniformBuffer> viewProjectionUniformBuffer;
		Ref<UniformBuffer> roughnessUniformBuffer;
		viewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * viewProjectionData.size(), 4);
		roughnessUniformBuffer = UniformBuffer::Create(sizeof(float), 5);

		unsigned int captureFBO, captureRBO;
		//glGenFramebuffers(1, &captureFBO);
		//glGenRenderbuffers(1, &captureRBO);

		//glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		//glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			//glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			roughnessUniformBuffer->SetData(&roughness, sizeof(float));

			for (unsigned int i = 0; i < 6; ++i)
			{
				viewProjectionData.at(1) = m_CaptureViews[i];
				viewProjectionUniformBuffer->SetData(viewProjectionData.data(), sizeof(glm::mat4) * viewProjectionData.size());
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				//	GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_PreFilteredRendererID, mip);

				//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Renderer::DrawCube();
			}
		}

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDeleteFramebuffers(1, &captureFBO);
		//glDeleteRenderbuffers(1, &captureRBO);
		viewProjectionUniformBuffer = nullptr;
		roughnessUniformBuffer = nullptr;
	}
}