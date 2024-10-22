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

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		std::string sectionName;
		bool open;

		sectionName = "Primary Camera";
		open = ImGui::TreeNodeEx(sectionName.c_str(), treeNodeFlags);
		if (open)
		{
			DrawPrimaryCamera();
			ImGui::TreePop();
		}

		ImGui::Separator();

		sectionName = "Skybox";
		open = ImGui::TreeNodeEx(sectionName.c_str(), treeNodeFlags);
		if (open)
		{
			DrawSkybox();
			ImGui::TreePop();
		}

		ImGui::End();
	}

	void SceneSettingsPanel::DrawPrimaryCamera()
	{
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

		if (ImGuiUtils::BeginCombo("Camera", *currentCameraTagString))
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
	}

	void SceneSettingsPanel::DrawSkybox()
	{
		const char* skyboxFacesStrings[] = { "Right Face", "Left Face", "Top Face", "Bottom Face", "Back Face", "Front Face" };

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		bool open;

		for (int i = 0; i < 6; i++)
		{
			open = ImGui::TreeNodeEx(skyboxFacesStrings[i], treeNodeFlags);
			if (open)
			{
				ImGuiUtils::BeginTextureViewer("Texture", m_Context->m_Skybox->GetFace((CubemapFace)i), 150.0, 150.0, true);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path payloadPath(path);

						Ref<Texture2D> texture = Texture2D::Create(payloadPath.string(), false);
						if (texture->IsLoaded())
						{
							m_Context->m_Skybox->SetFace((CubemapFace)i, texture);
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGuiUtils::EndTextureViewer(m_Context->m_Skybox->GetFace((CubemapFace)i)))
				{
					m_Context->m_Skybox->SetFace((CubemapFace)i, nullptr);
				}

				//if (m_Context->m_SkyBox[i] == nullptr)
				//{
				//	ImGuiUtils::ColorEdit3("Color", *glm::value_ptr(m_Context->m_SkyBoxColors[i]));
				//}

				ImGui::TreePop();
			}
		}
	}
}