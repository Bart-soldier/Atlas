#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Atlas/Scene/Components.h"

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
			DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			ImGui::Checkbox("Primary", &component.Primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
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

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
				{
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));
				}

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
				{
					camera.SetPerspectiveNearClip(perspectiveNear);
				}

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
				{
					camera.SetPerspectiveFarClip(perspectiveFar);
				}
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
				{
					camera.SetOrthographicSize(orthoSize);
				}

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear))
				{
					camera.SetOrthographicNearClip(orthoNear);
				}

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
				{
					camera.SetOrthographicFarClip(orthoFar);
				}

				ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
			}
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::Separator();

			//ImGui::Button("Texture", ImVec2(100.0f, 0.0f));


			//(ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)

			DrawTextureViewerPreDragDropTarget(component.Texture, 150.0, 150.0, true);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path payloadPath = std::filesystem::path(g_AssetPath) / path;

					Ref<Texture2D> texture = Texture2D::Create(payloadPath.string());
					if (texture->IsLoaded())
					{
						component.Texture = texture;
						component.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (DrawTextureViewerPostDragDropTarget(component.Texture))
			{
				component.Texture = nullptr;
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
		});
	}

	/* ------------------------------------------------- *
	 * --------------- CUSTOM UI HELPERS --------------- *
	 * ------------------------------------------------- */

	void SceneHierarchyPanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.569f, 0.220f, 0.231f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.137f, 0.431f, 0.220f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.271f, 0.590f, 0.900f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.180f, 0.384f, 0.588f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.271f, 0.590f, 0.900f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawTextureViewerPreDragDropTarget(const Ref<Texture2D> texture, float desiredWidth, float desiredHeight, bool flipped)
	{
		// Texture UV
		ImVec2 uv_min;                                   // Top-left
		ImVec2 uv_max;                                   // Lower-right
		if (flipped)
		{
			uv_min = ImVec2(0.0f, 1.0f);                 // Top-left
			uv_max = ImVec2(1.0f, 0.0f);                 // Lower-right
		}
		else
		{
			uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
			uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
		}

		// Colors
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

		if (texture == nullptr)
		{
			ImGui::Image(nullptr, ImVec2(desiredWidth, desiredHeight), uv_min, uv_max, tint_col, border_col);
		}
		else
		{
			ImTextureID textureID = (ImTextureID)texture->GetRendererID();
			float textureWidth = (float)texture->GetWidth();
			float textureHeight = (float)texture->GetHeight();
			float resizeFactorWidth = textureWidth / desiredWidth;
			float resizeFactorHeight = textureHeight / desiredHeight;

			{
				ImVec2 uiWindowPos = ImGui::GetCursorScreenPos();

				ImGui::Image(textureID, ImVec2(desiredWidth, desiredHeight), uv_min, uv_max, tint_col, border_col);
				if (ImGui::BeginItemTooltip())
				{
					float regionSize = 32.0f;
					float zoom = 4.0f;

					float regionX = ImGui::GetIO().MousePos.x - uiWindowPos.x - regionSize * 0.5f;
					if (regionX < 0.0f)
					{
						regionX = 0.0f;
					}
					else if (regionX > desiredWidth - regionSize)
					{
						regionX = desiredWidth - regionSize;
					}

					float regionY = ImGui::GetIO().MousePos.y - uiWindowPos.y - regionSize * 0.5f;
					if (regionY < 0.0f)
					{
						regionY = 0.0f;
					}
					else if (regionY > desiredHeight - regionSize)
					{
						regionY = desiredHeight - regionSize;
					}

					ImGui::Text("Min: (%d, %d)", (int)(regionX * resizeFactorWidth), (int)(regionY * resizeFactorHeight));
					ImGui::Text("Max: (%d, %d)", (int)((regionX + regionSize) * resizeFactorWidth), (int)((regionY + regionSize) * resizeFactorHeight));


					ImVec2 uv0;
					ImVec2 uv1;
					if (flipped)
					{
						float flippedY = desiredHeight - regionSize - regionY;
						uv0 = ImVec2((regionX) / desiredWidth, (flippedY + regionSize) / desiredHeight);
						uv1 = ImVec2((regionX + regionSize) / desiredWidth, (flippedY) / desiredHeight);
					}
					else
					{
						uv0 = ImVec2((regionX) / desiredWidth, (regionY) / desiredHeight);
						uv1 = ImVec2((regionX + regionSize) / desiredWidth, (regionY + regionSize) / desiredHeight);
					}

					ImGui::Image(textureID, ImVec2(regionSize * zoom, regionSize * zoom), uv0, uv1, tint_col, border_col);
					ImGui::EndTooltip();
				}
			}
		}
	}

	bool SceneHierarchyPanel::DrawTextureViewerPostDragDropTarget(const Ref<Texture2D> texture)
	{
		bool deleteTexture = false;

		if (texture != nullptr)
		{
			float nextY = ImGui::GetCursorPosY();
			ImGui::SameLine();
			float posX = ImGui::GetCursorPosX();
			float posY = ImGui::GetCursorPosY();
			float buttonSize = 25.0f;
			float padding = ImGui::GetStyle().FramePadding.y;

			ImGui::SetCursorPosX(posX + padding);
			if (ImGui::Button("X", ImVec2(buttonSize, buttonSize)))
			{
				deleteTexture = true;
			}

			ImGui::SetCursorPosX(posX + padding);
			ImGui::SetCursorPosY(posY + ImGui::GetTextLineHeightWithSpacing());
			ImGui::Text(texture->GetPath().c_str());

			ImGui::SetCursorPosX(posX + padding);
			ImGui::SetCursorPosY(posY + ImGui::GetTextLineHeightWithSpacing());
			ImGui::Text("\n%dx%d", texture->GetWidth(), texture->GetHeight());

			ImGui::SetCursorPosY(nextY);
		}

		return deleteTexture;
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