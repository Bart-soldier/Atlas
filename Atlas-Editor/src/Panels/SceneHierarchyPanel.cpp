#include "Panels/SceneHierarchyPanel.h"

#include "Atlas/Scene/Components.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Utils/ImGuiUtils.h"

namespace Atlas
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectedEntity = nullptr;
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity* entity)
	{
		m_SelectedEntity = entity;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			auto& name = m_Context->m_Name;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), name.c_str());
			if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}

			for (auto& [handle, entity] : m_Context->m_EntityHandleMap)
			{
				if (entity->GetParent() == nullptr)
				{
					DrawEntityNode(entity);
				}
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectedEntity = nullptr;
			}

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Entity"))
				{
					m_Context->CreateEntity("Untitled Entity");
				}

				ImGui::EndPopup();
			}

			ImGui::Dummy({ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y});

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
				{
					Entity* droppedEntity = (Entity*)payload->Data;
					m_Context->GetEntity(droppedEntity->GetHandle())->SetParent(nullptr);
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectedEntity)
		{
			DrawComponents(m_SelectedEntity);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity* entity)
	{
		auto& tag = entity->GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity->GetHandle(), flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				m_Context->CreateEntity("Untitled Entity", entity);
			}

			if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("SCENE_HIERARCHY_ENTITY", entity, sizeof(Entity));
			ImGui::EndDragDropSource();
		}

		if (opened)
		{
			for (Entity* child : entity->GetChildren())
			{
				DrawEntityNode(child);
			}

			ImGui::TreePop();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				Entity* droppedEntity = (Entity*)payload->Data;
				m_Context->GetEntity(droppedEntity->GetHandle())->SetParent(entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
			{
				m_SelectedEntity = nullptr;
			}
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity* entity)
	{
		ImVec2 padding = ImGui::GetStyle().FramePadding;
		ImVec2 buttonLabelSize = ImGui::CalcTextSize("Add Component", NULL, true);
		ImVec2 buttonSize = ImGui::CalcItemSize({0, 0}, buttonLabelSize.x + padding.x * 2.0f, buttonLabelSize.y + padding.y * 2.0f);

		ImGui::PushItemWidth(ImGui::GetColumnWidth() - buttonSize.x - padding.x);

		if (entity->HasComponent<TagComponent>())
		{
			auto& tag = entity->GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::PopItemWidth();

		ImGui::SameLine();

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<TransformComponent>("Transform");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntryIfOther<PostProcessorComponent, CameraComponent>("Post Processor");
			DisplayAddComponentEntryIfNoOther<SpriteRendererComponent, MeshComponent>("Sprite Renderer");
			DisplayAddComponentEntryIfNoOther<MeshComponent, SpriteRendererComponent>("Mesh");
			DisplayAddComponentEntryIfOther<MaterialComponent, MeshComponent>("Material");
			DisplayAddComponentEntry<LightSourceComponent>("Light Source");

			ImGui::EndPopup();
		}

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			ImGuiUtils::DragFloat3("Translation", component.Translation, 0.0f, 0.1f);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			ImGuiUtils::DragFloat3("Rotation", rotation, 0.0f, 0.1f);
			component.Rotation = glm::radians(rotation);
			ImGuiUtils::DragFloat3("Scale", component.Scale, 1.0f, 0.1f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			ImGui::Separator();

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGuiUtils::BeginCombo("Projection", *currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGuiUtils::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGuiUtils::DragFloat("Vertical FOV", perspectiveVerticalFov, 45.0f))
				{
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));
				}

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGuiUtils::DragFloat("Near", perspectiveNear, 0.01f))
				{
					camera.SetPerspectiveNearClip(perspectiveNear);
				}

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGuiUtils::DragFloat("Far", perspectiveFar, 1000.0f))
				{
					camera.SetPerspectiveFarClip(perspectiveFar);
				}
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGuiUtils::DragFloat("Size", orthoSize, 10.0f))
				{
					camera.SetOrthographicSize(orthoSize);
				}

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGuiUtils::DragFloat("Near", orthoNear, -1.0f))
				{
					camera.SetOrthographicNearClip(orthoNear);
				}

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGuiUtils::DragFloat("Far", orthoFar, 1.0f))
				{
					camera.SetOrthographicFarClip(orthoFar);
				}

				ImGuiUtils::Checkbox("Fixed Aspect Ratio", component.FixedAspectRatio);
			}
		});

		DrawComponent<PostProcessorComponent>("Post Processor", entity, [](auto& component)
		{
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
			const char* effectsStrings[] = { "None", "Inversion", "Greyscale", "Numerical", "Blur", "Edge Detection"};
			int removeEffect = -1;

			for (int effectIndex = 0; effectIndex < component.Effects.size(); effectIndex++)
			{
				if (effectIndex > 0)
				{
					ImGui::Separator();
				}

				const char* currentEffectString = effectsStrings[(int)component.Effects[effectIndex]];
				if (ImGuiUtils::BeginCombo("Effect", *currentEffectString, ImGui::GetContentRegionAvail().x - buttonSize.x, effectIndex))
				{
					for (int stringIndex = 0; stringIndex < 6; stringIndex++)
					{
						bool isSelected = currentEffectString == effectsStrings[stringIndex];
						if (ImGui::Selectable(effectsStrings[stringIndex], isSelected))
						{
							currentEffectString = effectsStrings[stringIndex];
							component.Effects[effectIndex] = (PostProcessor::PostProcessingEffect)stringIndex;

							if (component.Effects[effectIndex] == PostProcessor::PostProcessingEffect::Numerical)
							{
								component.KernelOffsets[effectIndex] = 90.0f;
							}

							if (component.Effects[effectIndex] == PostProcessor::PostProcessingEffect::Blur ||
								component.Effects[effectIndex] == PostProcessor::PostProcessingEffect::EdgeDetection)
							{
								component.KernelOffsets[effectIndex] = 300.0f;
							}
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGuiUtils::EndCombo();
				}
				//ImGui::PopItemWidth();

				//ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight * 0.5f);
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - buttonSize.x);
				//ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - buttonSize.x);
				if (ImGui::Button(("-##" + std::to_string(effectIndex)).c_str(), buttonSize))
				{
					removeEffect = effectIndex;
				}

				if (component.Effects[effectIndex] == PostProcessor::PostProcessingEffect::Numerical)
				{
					ImGuiUtils::DragFloat("Offset", component.KernelOffsets[effectIndex], 90.0f, 1.0f, 1.0f);
				}

				if (component.Effects[effectIndex] == PostProcessor::PostProcessingEffect::Blur ||
					component.Effects[effectIndex] == PostProcessor::PostProcessingEffect::EdgeDetection)
				{
					ImGuiUtils::DragFloat("Offset", component.KernelOffsets[effectIndex], 300.0f, 1.0f, 1.0f);
				}
			}

			ImGui::Separator();

			//ImVec2 padding = ImGui::GetStyle().FramePadding;
			//ImVec2 buttonLabelSize = ImGui::CalcTextSize("Add Effect", NULL, true);
			//ImVec2 buttonSize = ImGui::CalcItemSize({ 0, 0 }, buttonLabelSize.x + padding.x * 2.0f, buttonLabelSize.y + padding.y * 2.0f);

			//ImGui::PushItemWidth(2 * buttonSize.x);

			if (removeEffect >= 0)
			{
				component.Effects.erase(component.Effects.begin() + removeEffect);
				component.KernelOffsets.erase(component.KernelOffsets.begin() + removeEffect);
			}

			if (ImGui::Button("Add Effect"))
			{
				component.Effects.push_back(PostProcessor::PostProcessingEffect::None);
				component.KernelOffsets.push_back(0.0f);
			}

			//ImGui::PopItemWidth();
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGuiUtils::ColorEdit4("Color", *glm::value_ptr(component.Color));

			const char* renderTypeStrings[] = { "Square", "Circle" };
			const char* currentRenderTypeString = renderTypeStrings[(int)component.Type];
			if (ImGuiUtils::BeginCombo("Render Type", *currentRenderTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentRenderTypeString == renderTypeStrings[i];
					if (ImGui::Selectable(renderTypeStrings[i], isSelected))
					{
						currentRenderTypeString = renderTypeStrings[i];
						component.Type = ((SpriteRendererComponent::RenderType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGuiUtils::EndCombo();
			}

			ImGui::Separator();

			if (component.Type == SpriteRendererComponent::RenderType::Square)
			{
				ImGuiUtils::BeginTextureViewer("Texture", component.Texture, 150.0, 150.0, true);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path payloadPath(path);

						Ref<Texture2D> texture = Texture2D::Create(payloadPath.string());
						if (texture->IsLoaded())
						{
							component.Texture = texture;
							component.SpriteSheet = false;
							component.SubTexture = nullptr;
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGuiUtils::EndTextureViewer(component.Texture))
				{
					component.ResetTextureValues();
				}

				if (component.Texture)
				{
					ImGuiUtils::DragFloat("Tiling Factor", component.TilingFactor, 1.0f, 0.1f, 0.0f, 100.0f);
					if (ImGuiUtils::Checkbox("Sprite Sheet", component.SpriteSheet))
					{
						component.SubTexture = component.SpriteSheet ? SubTexture2D::CreateFromCoords(component.Texture, component.SubTextureCoords, component.SubTextureCellSize, component.SubTextureSpriteSize)
																     : nullptr;
					}

					if (component.SpriteSheet)
					{
						if (ImGuiUtils::DragFloat2("Sprite Coords", component.SubTextureCoords, 0.0f) ||
							ImGuiUtils::DragFloat2("Sprite Cell Size", component.SubTextureCellSize, 32.0f, 16.0f) ||
							ImGuiUtils::DragFloat2("Sprite Size", component.SubTextureSpriteSize, 1.0f))
						{
							Ref<SubTexture2D> subTexture = SubTexture2D::CreateFromCoords(component.Texture, component.SubTextureCoords, component.SubTextureCellSize, component.SubTextureSpriteSize);
							component.SubTexture = subTexture;
						}
					}
				}
			}

			if (component.Type == SpriteRendererComponent::RenderType::Circle)
			{
				ImGuiUtils::DragFloat("Thickness", component.Thickness, 1.0f, 0.025f, 0.0f, 1.0f);
				ImGuiUtils::DragFloat("Fade", component.Fade, 0.005f, 0.00025f, 0.0f, 1.0f);
			}
		});

		DrawComponent<MeshComponent>("Mesh", entity, [](auto& component)
		{
			auto& mesh = component.Mesh;

			const char* meshPresetStrings[] = {
				"Custom",
				"Square",
			};
			const char* currentMeshPresetString = meshPresetStrings[(int)mesh->GetMeshPreset()];
			if (ImGuiUtils::BeginCombo("Presets", *currentMeshPresetString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentMeshPresetString == meshPresetStrings[i];
					if (ImGui::Selectable(meshPresetStrings[i], isSelected))
					{
						currentMeshPresetString = meshPresetStrings[i];
						mesh->SetMeshPreset((Mesh::MeshPresets)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGuiUtils::EndCombo();
			}
		});

		DrawComponent<MaterialComponent>("Material", entity, [](auto& component)
		{
			auto& material = component.Material;

			const char* materialPresetStrings[] = {
				"Custom",
				"Default",
				"Emerald",
				"Jade",
				"Obsidian",
				"Pearl",
				"Ruby",
				"Turquoise",
				"Brass",
				"Bronze",
				"Chrome",
				"Copper",
				"Gold",
				"Silver",
				"Black Plastic",
				"Cyan Plastic",
				"Green Plastic",
				"Red Plastic",
				"White Plastic",
				"Yellow Plastic",
				"Black Rubber",
				"Cyan Rubber",
				"Green Rubber",
				"Red Rubber",
				"White Rubber",
				"Yellow Rubber"
			};
			const char* currentMaterialPresetString = materialPresetStrings[(int)material->GetMaterialPreset()];
			if (ImGuiUtils::BeginCombo("Presets", *currentMaterialPresetString))
			{
				for (int i = 0; i < 26; i++)
				{
					bool isSelected = currentMaterialPresetString == materialPresetStrings[i];
					if (ImGui::Selectable(materialPresetStrings[i], isSelected))
					{
						currentMaterialPresetString = materialPresetStrings[i];
						material->SetMaterialPreset((Material::MaterialPresets)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGuiUtils::EndCombo();
			}

			// Diffuse component
			ImGuiUtils::BeginTextureViewer("Diffuse Texture", material->GetDiffuseTexture(), 150.0, 150.0, true);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path payloadPath(path);

					Ref<Texture2D> texture = Texture2D::Create(payloadPath.string());
					if (texture->IsLoaded())
					{
						material->SetDiffuseTexture(texture);
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGuiUtils::EndTextureViewer(material->GetDiffuseTexture()))
			{
				material->SetDiffuseTexture(nullptr);
			}

			if (material->GetDiffuseTexture() == nullptr)
			{
				glm::vec3 ambientColor = material->GetAmbientColor();
				if (ImGuiUtils::ColorEdit3("Ambient Color", *glm::value_ptr(ambientColor)))
				{
					material->SetAmbientColor(ambientColor);
				}

				glm::vec3 diffuseColor = material->GetDiffuseColor();
				if (ImGuiUtils::ColorEdit3("Diffuse Color", *glm::value_ptr(diffuseColor)))
				{
					material->SetDiffuseColor(diffuseColor);
				}
			}

			// Specular component
			ImGuiUtils::BeginTextureViewer("Specular Texture", material->GetSpecularTexture(), 150.0, 150.0, true);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path payloadPath(path);

					Ref<Texture2D> texture = Texture2D::Create(payloadPath.string());
					if (texture->IsLoaded())
					{
						material->SetSpecularTexture(texture);
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGuiUtils::EndTextureViewer(material->GetSpecularTexture()))
			{
				material->SetSpecularTexture(nullptr);
			}

			if (material->GetSpecularTexture() == nullptr)
			{
				glm::vec3 specularColor = material->GetSpecularColor();
				if (ImGuiUtils::ColorEdit3("Specular Color", *glm::value_ptr(specularColor)))
				{
					material->SetSpecularColor(specularColor);
				}
			}

			float shininess = material->GetShininess();
			if (ImGuiUtils::DragFloat("Shininess", shininess, 1.0f, 0.001f, 0.0f, 1.0f))
			{
				material->SetShininess(shininess);
			}
		});

		DrawComponent<LightSourceComponent>("Light Source", entity, [](auto& component)
		{
			auto& light = component.Light;

			glm::vec3 color = light->GetColor();
			if (ImGuiUtils::ColorEdit3("Color", *glm::value_ptr(color)))
			{
				light->SetColor(color);
			}

			float intensity = light->GetIntensity();
			if (ImGuiUtils::DragFloat("Intensity", intensity, 1.0f, 1.0f, 0.0f))
			{
				light->SetIntensity(intensity);
			}

			float ambientStrength = light->GetAmbientStrength();
			if (ImGuiUtils::DragFloat("Ambient Strength", ambientStrength, 0.1f, 0.001f, 0.0f, 1.0f))
			{
				light->SetAmbientStrength(ambientStrength);
			}

			float diffuseStrength = light->GetDiffuseStrength();
			if (ImGuiUtils::DragFloat("Diffuse Strength", diffuseStrength, 0.5f, 0.001f, 0.0f, 1.0f))
			{
				light->SetDiffuseStrength(diffuseStrength);
			}

			float specularStrength = light->GetSpecularStrength();
			if (ImGuiUtils::DragFloat("Specular Strength", specularStrength, 1.0f, 0.001f, 0.0f, 1.0f))
			{
				light->SetSpecularStrength(specularStrength);
			}

			ImGui::Separator();

			const char* castTypeStrings[] = { "Directional Light", "Point Light", "Spotlight"};
			const char* currentCastTypeString = castTypeStrings[(int)light->GetCastType()];
			if (ImGuiUtils::BeginCombo("Cast Type", *currentCastTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentCastTypeString == castTypeStrings[i];
					if (ImGui::Selectable(castTypeStrings[i], isSelected))
					{
						currentCastTypeString = castTypeStrings[i];
						light->SetCastType((Light::CastType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGuiUtils::EndCombo();
			}

			if (light->GetCastType() != Light::CastType::DirectionalLight)
			{
				float range = light->GetRadius();
				if (ImGuiUtils::DragFloat("Radius", range, 0.01f, 0.01f, 0.0001f))
				{
					light->SetRadius(range);
				}
			}

			if (light->GetCastType() == Light::CastType::Spotlight)
			{
				glm::vec2 cutOff = light->GetCutOff();
				if (ImGuiUtils::DragFloat2("Cut-off", cutOff, 10.00f, 0.1f, 0.0001f))
				{
					light->SetCutOff(cutOff);
				}
			}
		});
	}

	template<typename T, typename UIFunction>
	void SceneHierarchyPanel::DrawComponent(const std::string& name, Entity* entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity->HasComponent<T>())
		{
			auto& component = entity->GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (!std::is_same<T, TransformComponent>::value && ImGui::MenuItem("Remove component"))
				{
					removeComponent = true;
				}

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity->RemoveComponent<T>();
			}
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!m_SelectedEntity->HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectedEntity->AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	template<typename T, typename T2>
	void SceneHierarchyPanel::DisplayAddComponentEntryIfOther(const std::string& entryName)
	{
		if (m_SelectedEntity->HasComponent<T2>())
		{
			DisplayAddComponentEntry<T>(entryName);
		}
	}

	template<typename T, typename T2>
	void SceneHierarchyPanel::DisplayAddComponentEntryIfNoOther(const std::string& entryName)
	{
		if (!m_SelectedEntity->HasComponent<T2>())
		{
			DisplayAddComponentEntry<T>(entryName);
		}
	}
}