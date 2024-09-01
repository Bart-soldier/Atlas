#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
}

void Sandbox2D::OnAttach()
{
	ATLAS_PROFILE_FUNCTION();

	//m_CheckerboardTexture = Atlas::Texture2D::Create("assets/textures/Checkerboard.png");
	m_SpriteSheet = Atlas::Texture2D::Create("assets/textures/SpriteSheet.png");
	m_TextureStairs = Atlas::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
	m_TextureBarrel = Atlas::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 });
	m_TextureTree = Atlas::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 1 }, { 128, 128 }, {1, 2});

	m_CameraController.SetZoomLevel(5.0f);
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
	Atlas::Renderer2D::ResetStats();
	{
		ATLAS_PROFILE_SCOPE("Renderer Prep");
		Atlas::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Atlas::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		ATLAS_PROFILE_SCOPE("Renderer Draw");

		/*Atlas::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Atlas::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(rotation), { 0.8f, 0.2f, 0.3f, 1.0f });
		Atlas::Renderer2D::DrawQuad({ 2.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Atlas::Renderer2D::DrawQuad({ 0.0f,-2.0f }, { 0.5f, 0.75f }, m_SquareColor);
		//Atlas::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);

		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Atlas::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		
		Atlas::Renderer2D::EndScene();*/

		Atlas::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Atlas::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStairs);
		Atlas::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureBarrel);
		Atlas::Renderer2D::DrawQuad({-1.0f, 0.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);
		Atlas::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ATLAS_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	
	auto stats = Atlas::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quad Count: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));	
	ImGui::End();
}

void Sandbox2D::OnEvent(Atlas::Event& e)
{
	m_CameraController.OnEvent(e);
}