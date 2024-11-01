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
		ImGuiUtils::BeginTextureViewer("HDRI", m_Context->m_Skybox->GetMap(), 150.0, 150.0, true);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path payloadPath(path);

				if (payloadPath.extension() == ".hdr")
				{
					Ref<Texture2D> texture = Texture2D::Create(payloadPath.string(), false);
					if (texture->IsLoaded())
					{
						m_Context->m_Skybox->SetMap(texture);
					}
				}
				else
				{
					ATLAS_CORE_WARN("Only '.hdr' images are supported for skyboxes - {0}", payloadPath.string());
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGuiUtils::EndTextureViewer(m_Context->m_Skybox->GetMap()))
		{
			m_Context->m_Skybox->SetMap(nullptr);
		}

		const char* skyboxTypeStrings[] = { "None", "Cubemap", "Irradiance Map", "Pre-filtered Map" };
		const char* currentSkyboxTypeString = skyboxTypeStrings[(int)Renderer::GetSkyboxType()];
		if (ImGuiUtils::BeginCombo("Skybox Type", *currentSkyboxTypeString))
		{
			for (int i = 0; i < 4; i++)
			{
				bool isSelected = currentSkyboxTypeString == skyboxTypeStrings[i];
				if (ImGui::Selectable(skyboxTypeStrings[i], isSelected))
				{
					currentSkyboxTypeString = skyboxTypeStrings[i];
					Renderer::SetSkyboxType((Cubemap::MapType)i);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGuiUtils::EndCombo();
		}
	}

	void SceneSettingsPanel::DrawGraphicsSettings()
	{
		const char* bufferTypeStrings[] = { "Final", "EntityID", "Position", "Normal", "Albedo", "Material", "BrightColors", "SSAO" };
		const char* currentBufferTypeString = bufferTypeStrings[(int)Renderer::GetDisplayedBuffer()];
		if (ImGuiUtils::BeginCombo("Render Buffer", *currentBufferTypeString))
		{
			for (int i = 0; i < 8; i++)
			{
				bool isSelected = currentBufferTypeString == bufferTypeStrings[i];
				if (ImGui::Selectable(bufferTypeStrings[i], isSelected))
				{
					currentBufferTypeString = bufferTypeStrings[i];
					Renderer::SetDisplayedBuffer((Renderer::RenderBuffers)i);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGuiUtils::EndCombo();
		}

		ImGui::Separator();

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

		bool isSSAOEnabled = Renderer::IsSSAOEnabled();
		if (ImGuiUtils::Checkbox("SSAO", isSSAOEnabled))
		{
			Renderer::ToggleSSAO();
		}

		ImGui::Separator();

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