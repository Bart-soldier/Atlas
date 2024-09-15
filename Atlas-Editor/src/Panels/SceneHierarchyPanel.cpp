#include "Panels/SceneHierarchyPanel.h"

#include "Atlas/Scene/Components.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Atlas
{
	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
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

			for (auto entityID : m_Context->m_Registry.view<entt::entity>())
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectionContext = {};
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
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
			{
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
			{
				m_SelectionContext = {};
			}
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<TransformComponent>("Transform");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

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

			ImGuiUtils::Checkbox("Primary", component.Primary);

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
				ImGuiUtils::DrawTextureViewerPreDragDropTarget(component.Texture, 150.0, 150.0, true);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path payloadPath = std::filesystem::path(g_AssetPath) / path;

						Ref<Texture2D> texture = Texture2D::Create(payloadPath.string());
						if (texture->IsLoaded())
						{
							component.ResetTextureValues();
							component.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

							component.Texture = texture;
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGuiUtils::DrawTextureViewerPostDragDropTarget(component.Texture))
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

				
				//bool SpriteSheet = false;
				//glm::vec2 SubTextureCoords = { 0, 0 };
				//glm::vec2 SubTextureCellSize = { 32, 32 };
				//glm::vec2 SubTextureSpriteSize = { 1, 1 };
			}

			if (component.Type == SpriteRendererComponent::RenderType::Circle)
			{
				ImGuiUtils::DragFloat("Thickness", component.Thickness, 1.0f, 0.025f, 0.0f, 1.0f);
				ImGuiUtils::DragFloat("Fade", component.Fade, 0.005f, 0.00025f, 0.0f, 1.0f);
			}
		});
	}

	template<typename T, typename UIFunction>
	void SceneHierarchyPanel::DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
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
				if (ImGui::MenuItem("Remove component"))
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
				entity.RemoveComponent<T>();
			}
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
}