#include <Atlas.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public Atlas::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Atlas::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<Atlas::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Atlas::VertexBuffer::Create(vertices, sizeof(vertices)));
		vertexBuffer->SetLayout({
			{ Atlas::ShaderDataType::Float3, "a_Position" },
			{ Atlas::ShaderDataType::Float4, "a_Color" }
		});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Atlas::IndexBuffer> indexBuffer;
		indexBuffer.reset(Atlas::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Atlas::VertexArray::Create());

		float squareVertices[4 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::shared_ptr<Atlas::VertexBuffer> squareVB;
		squareVB.reset(Atlas::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Atlas::ShaderDataType::Float3, "a_Position" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Atlas::IndexBuffer> squareIB;
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

		m_Shader.reset(new Atlas::Shader(vertexSrc, fragmentSrc));

		std::string blueVertexSrc = R"(
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

			uniform vec4 u_Color;

			in vec3 v_Position;

			void main()
			{
				color = u_Color;
			}
		)";

		m_FlatColorShader.reset(new Atlas::Shader(blueVertexSrc, flatColorFragmentSrc));

	}

	void OnUpdate(Atlas::Timestep ts) override
	{
		if (Atlas::Input::IsKeyPressed(ATLAS_KEY_A))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}
		else if (Atlas::Input::IsKeyPressed(ATLAS_KEY_D))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}
		if (Atlas::Input::IsKeyPressed(ATLAS_KEY_W))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}
		else if (Atlas::Input::IsKeyPressed(ATLAS_KEY_S))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}

		if (Atlas::Input::IsKeyPressed(ATLAS_KEY_Q))
		{
			m_CameraRotation += m_CameraRotationSpeed * ts;
		}

		if (Atlas::Input::IsKeyPressed(ATLAS_KEY_E))
		{
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		}

		Atlas::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Atlas::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Atlas::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
		glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				
				if (x % 2 == 0)
					m_FlatColorShader->UploadUniformFloat4("u_Color", redColor);
				else
					m_FlatColorShader->UploadUniformFloat4("u_Color", blueColor);
				
				Atlas::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		Atlas::Renderer::Submit(m_Shader, m_VertexArray);

		Atlas::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(Atlas::Event& event) override
	{

	}

private:

	std::shared_ptr<Atlas::Shader> m_Shader;
	std::shared_ptr<Atlas::VertexArray> m_VertexArray;

	std::shared_ptr<Atlas::Shader> m_FlatColorShader;
	std::shared_ptr<Atlas::VertexArray> m_SquareVA;

	Atlas::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 3.0f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 90.0f;
};

class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}