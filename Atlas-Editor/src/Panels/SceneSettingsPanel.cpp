#include "Panels/SceneSettingsPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Utils/ImGuiUtils.h"

#include "Atlas/Scene/Entity.h"

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

		std::vector<Entity*> cameras = m_Context->GetCameras();

		std::vector<const char*> cameraTagStrings;
		cameraTagStrings.reserve(cameras.size() + 1);
		cameraTagStrings.push_back("None");
		const char* currentCameraTagString = cameraTagStrings[0];

		for (int i = 0; i < cameras.size(); i++)
		{
			Entity* camera = cameras[i];

			cameraTagStrings.push_back(camera->GetComponent<TagComponent>().Tag.c_str());
			if (camera == m_Context->GetPrimaryCamera())
			{
				currentCameraTagString = cameraTagStrings[i + 1];
			}
		}

		if (ImGuiUtils::BeginCombo("Primary Camera", *currentCameraTagString))
		{
			for (int i = 0; i < cameraTagStrings.size(); i++)
			{
				bool isSelected = currentCameraTagString == cameraTagStrings[i];
				if (ImGui::Selectable(cameraTagStrings[i], isSelected))
				{
					currentCameraTagString = cameraTagStrings[i];
					m_Context->SetPrimaryCamera(i == 0 ? nullptr : cameras[i - 1]);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGuiUtils::EndCombo();
		}

		ImGui::End();
	}
}