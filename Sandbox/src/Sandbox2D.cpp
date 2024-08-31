#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
}

void Sandbox2D::OnAttach()
{
	ATLAS_PROFILE_FUNCTION();

	m_CheckerboardTexture = Atlas::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	ATLAS_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Atlas::Timestep ts)
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

		Atlas::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Atlas::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Atlas::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		Atlas::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);

		Atlas::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ATLAS_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));	
	ImGui::End();
}

void Sandbox2D::OnEvent(Atlas::Event& e)
{
	m_CameraController.OnEvent(e);
}