#pragma once

#include "Atlas.h"

class Sandbox3D : public Atlas::Layer
{
public:
	Sandbox3D();
	virtual ~Sandbox3D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Atlas::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Atlas::Event& e) override;

private:
	Atlas::ShaderLibrary m_ShaderLibrary;
	Atlas::Ref<Atlas::Shader> m_Shader;
	Atlas::Ref<Atlas::VertexArray> m_VertexArray;

	Atlas::Ref<Atlas::Shader> m_FlatColorShader;
	Atlas::Ref<Atlas::VertexArray> m_SquareVA;

	Atlas::Ref<Atlas::Texture2D> m_Texture, m_LogoTexture;

	Atlas::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};