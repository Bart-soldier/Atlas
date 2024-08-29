#pragma once

#include "Atlas.h"

class Sandbox2D : public Atlas::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Atlas::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Atlas::Event& e) override;
private:
	Atlas::OrthographicCameraController m_CameraController;

	// Temp
	Atlas::Ref<Atlas::VertexArray> m_SquareVA;
	Atlas::Ref<Atlas::Shader> m_FlatColorShader;

	Atlas::Ref<Atlas::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	struct ProfileResult
	{
		const char* Name;
		float Time;
	};

	std::vector<ProfileResult> m_ProfileResults;
};

