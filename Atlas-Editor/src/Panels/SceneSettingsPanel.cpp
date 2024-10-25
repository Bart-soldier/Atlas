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

		sectionName = "Graphics Settings";
		open = ImGui::TreeNodeEx(sectionName.c_str(), treeNodeFlags);
		if (open)
		{
			DrawGraphicsSettings();
			ImGui::TreePop();
		}

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

				ImGui::TreePop();
			}
		}
	}

	void SceneSettingsPanel::DrawGraphicsSettings()
	{
		bool isFlatShaderEnabled = Renderer::IsFlatShaderEnabled();
		if (ImGuiUtils::Checkbox("Flat Shader", isFlatShaderEnabled))
		{
			Renderer::ToggleFlatShader();
		}

		bool isHDREnabled = Renderer::IsHDREnabled();
		if (ImGuiUtils::Checkbox("HDR", isHDREnabled))
		{
			Renderer::ToggleHDR();
		}

		bool isBloomEnabled = Renderer::IsBloomEnabled();
		if (ImGuiUtils::Checkbox("Bloom", isBloomEnabled))
		{
			Renderer::ToggleBloom();
		}

		if (ImGuiUtils::Checkbox("Gamma Correction", m_GammaCorrection))
		{
			Renderer::SetGamma(m_GammaCorrection ? 2.2f : 1.0f);
		}

		float exposure = Renderer::GetExposure();
		if (ImGuiUtils::DragFloat("Exposure", exposure, 1.0f, 0.1f))
		{
			Renderer::SetExposure(exposure);
		}

		float parallaxScale = Renderer::GetParallaxScale();
		if (ImGuiUtils::DragFloat("Parallax Scale", parallaxScale, 0.1f, 0.01f))
		{
			Renderer::SetParallaxScale(parallaxScale);
		}

		float bloomThreshold = Renderer::GetBloomThreshold();
		if (ImGuiUtils::DragFloat("Bloom Threshold", bloomThreshold, 1.0f, 0.01f))
		{
			Renderer::SetBloomThreshold(bloomThreshold);
		}
	}
}