#include "Sandbox3D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Sandbox3D::Sandbox3D()
	: Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
{
	m_VertexArray = Atlas::VertexArray::Create();

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};

	Atlas::Ref<Atlas::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(Atlas::VertexBuffer::Create(vertices, sizeof(vertices)));
	vertexBuffer->SetLayout({
		{ Atlas::ShaderDataType::Float3, "a_Position" },
		{ Atlas::ShaderDataType::Float4, "a_Color" }
		});
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	Atlas::Ref<Atlas::IndexBuffer> indexBuffer;
	indexBuffer.reset(Atlas::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA = Atlas::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	Atlas::Ref<Atlas::VertexBuffer> squareVB;
	squareVB.reset(Atlas::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout({
		{ Atlas::ShaderDataType::Float3, "a_Position" },
		{ Atlas::ShaderDataType::Float2, "a_TexCoord" }
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Atlas::Ref<Atlas::IndexBuffer> squareIB;
	squareIB.reset(Atlas::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);

	std::string vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec3 v_Position;
		out vec4 v_Color;

		void main()
		{
			v_Position = a_Position;
			v_Color = a_Color;
			gl_Position = u_ViewProjection * u_Transform * vec4(v_Position, 1.0);
		}
	)";

	std::string fragmentSrc = R"(
		#version 330 core

		layout(location = 0) out vec4 color;

		in vec3 v_Position;
		in vec4 v_Color;

		void main()
		{
			color = vec4(v_Position * 0.5 + 0.5, 1.0);
			color = v_Color;
		}
	)";

	m_Shader = Atlas::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

	std::string flatColorVertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_Position;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec3 v_Position;

		void main()
		{
			v_Position = a_Position;
			gl_Position = u_ViewProjection * u_Transform * vec4(v_Position, 1.0);
		}
	)";

	std::string flatColorFragmentSrc = R"(
		#version 330 core

		layout(location = 0) out vec4 color;

		uniform vec3 u_Color;

		in vec3 v_Position;

		void main()
		{
			color = vec4(u_Color, 1.0);
		}
	)";

	m_FlatColorShader = Atlas::Shader::Create("FlatColor", flatColorVertexSrc, flatColorFragmentSrc);

	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	m_Texture = Atlas::Texture2D::Create("assets/textures/Checkerboard.png");
	m_LogoTexture = Atlas::Texture2D::Create("assets/textures/ChernoLogo.png");

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0);
}

void Sandbox3D::OnAttach()
{
	ATLAS_PROFILE_FUNCTION();
}

void Sandbox3D::OnDetach()
{
	ATLAS_PROFILE_FUNCTION();
}

void Sandbox3D::OnUpdate(Atlas::Timestep ts)
{
	ATLAS_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	{
		ATLAS_PROFILE_SCOPE("Renderer Prep");

		Atlas::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Atlas::RenderCommand::Clear();
	}

	{
		ATLAS_PROFILE_SCOPE("Renderer Draw");

		Atlas::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_FlatColorShader->Bind();
		m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Atlas::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Atlas::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5)));

		m_LogoTexture->Bind();
		Atlas::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5)));

		// Triangle
		// Atlas::Renderer::Submit(m_Shader, m_VertexArray);

		Atlas::Renderer::EndScene();
	}
}

void Sandbox3D::OnImGuiRender()
{
	ATLAS_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox3D::OnEvent(Atlas::Event& e)
{
	m_CameraController.OnEvent(e);
}