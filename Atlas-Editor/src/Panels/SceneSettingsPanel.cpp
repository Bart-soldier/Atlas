#include "Panels/SceneSettingsPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Utils/ImGuiUtils.h"

namespace Atlas
{
	SceneSettingsPanel::SceneSettingsPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneSettingsPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneSettingsPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Settings");

		glm::vec3 ambientLightColor = m_Context->GetAmbientLightColor();
		if (ImGuiUtils::ColorEdit3("Ambient Light Color", *glm::value_ptr(ambientLightColor)))
		{
			m_Context->SetAmbientLightColor(ambientLightColor);
		}

		float ambientLightIntensity = m_Context->GetAmbientLightIntensity();
		if (ImGuiUtils::DragFloat("Ambient Light Intensity", ambientLightIntensity, 0.1f, 0.001f, 0.0f, 1.0f))
		{
			m_Context->SetAmbientLightIntensity(ambientLightIntensity);
		}

		ImGui::End();
	}
}