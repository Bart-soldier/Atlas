#pragma once

#include "Atlas/Core/Layer.h"

#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Events/KeyEvent.h"
#include "Atlas/Events/MouseEvent.h"

namespace Atlas
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};
}