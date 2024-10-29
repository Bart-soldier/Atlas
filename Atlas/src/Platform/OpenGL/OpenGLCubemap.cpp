#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLCubemap.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Renderer/UniformBuffer.h"
#include "Atlas/Renderer/Framebuffer.h"

#include <stb_image.h>

namespace Atlas
{
	OpenGLCubemap::OpenGLCubemap()
	{
		ATLAS_PROFILE_FUNCTION();

		m_MapToCubemapShader        = Shader::Create("assets/shaders/Cubemap/Cubemap_Vert.glsl", "assets/shaders/Cubemap/MapToCubemap_Frag.glsl"       );
		m_CubemapToIrradianceShader = Shader::Create("assets/shaders/Cubemap/Cubemap_Vert.glsl", "assets/shaders/Cubemap/CubemapToIrradiance_Frag.glsl");

		CreateMap(&m_CubemapRendererID);
		CreateMap(&m_IrradianceRendererID);

		m_CaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		m_CaptureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
		m_CaptureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
		m_CaptureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
		m_CaptureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
		m_CaptureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
		m_CaptureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
	}

	void OpenGLCubemap::CreateMap(uint32_t* rendererID)
	{
		glGenTextures(1, rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, *rendererID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		ResetMap(rendererID);
	}

	void OpenGLCubemap::ResetMap(uint32_t* rendererID)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, *rendererID);

		for (int i = 0; i < 6; i++)
		{
			uint32_t blackTextureData = 0x00000000;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &blackTextureData);
		}
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		ATLAS_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_CubemapRendererID);
	}

	void OpenGLCubemap::SetMap(const Ref<Texture2D>& cubemap)
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
		}
		else
		{
			ResetMap(&m_CubemapRendererID);
			ResetMap(&m_IrradianceRendererID);
		}
	}

	void OpenGLCubemap::BindCubemap() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapRendererID);
	}

	void OpenGLCubemap::BindIrradianceMap() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceRendererID);
	}

	void OpenGLCubemap::LoadCubemap()
	{
		BindCubemap();

		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		m_MapToCubemapShader->Bind();
		m_Map->Bind();

		std::vector<glm::mat4> viewProjectionData;
		viewProjectionData.reserve(2);
		viewProjectionData.push_back(m_CaptureProjection);
		viewProjectionData.push_back(m_CaptureViews[0]);

		Ref<UniformBuffer> viewProjectionUniformBuffer;
		viewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * viewProjectionData.size(), 4);

		glViewport(0, 0, 512, 512);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			viewProjectionData.at(1) = m_CaptureViews[i];
			viewProjectionUniformBuffer->SetData(viewProjectionData.data(), sizeof(glm::mat4) * viewProjectionData.size());

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_CubemapRendererID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::DrawCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		viewProjectionUniformBuffer = nullptr;
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}

	void OpenGLCubemap::LoadIrradianceMap()
	{
		BindIrradianceMap();

		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		m_CubemapToIrradianceShader->Bind();
		BindCubemap();

		std::vector<glm::mat4> viewProjectionData;
		viewProjectionData.reserve(2);
		viewProjectionData.push_back(m_CaptureProjection);
		viewProjectionData.push_back(m_CaptureViews[0]);

		Ref<UniformBuffer> viewProjectionUniformBuffer;
		viewProjectionUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4) * viewProjectionData.size(), 4);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			viewProjectionData.at(1) = m_CaptureViews[i];
			viewProjectionUniformBuffer->SetData(viewProjectionData.data(), sizeof(glm::mat4) * viewProjectionData.size());

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceRendererID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::DrawCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		viewProjectionUniformBuffer = nullptr;
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}
}