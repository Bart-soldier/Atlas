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
	Atlas::Ref<Atlas::Framebuffer> m_Framebuffer;

	//Atlas::Ref<Atlas::Texture2D> m_CheckerboardTexture;
	Atlas::Ref<Atlas::Texture2D> m_SpriteSheet;
	Atlas::Ref<Atlas::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

	glm::vec4 m_SquareColor;
};

