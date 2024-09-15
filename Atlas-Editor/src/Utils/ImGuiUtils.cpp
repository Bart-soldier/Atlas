#include "ImGuiUtils.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Atlas
{
	bool ImGuiUtils::Checkbox(const std::string& label, bool& value)
	{
		bool valueChanged = false;

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 4);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		valueChanged = ImGui::Checkbox("##X", &value);

		ImGui::Columns(1);
		ImGui::PopID();

		return valueChanged;
	}

	bool ImGuiUtils::DragFloat(const std::string& label, float& value, float resetValue, float speed, float min, float max)
	{
		bool valueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 4);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::GetColumnWidth() - 1.4 * buttonSize.x);
		valueChanged = ImGui::DragFloat("##X", &value, speed, min, max, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - buttonSize.x);
		ImGui::PushFont(boldFont);
		if (ImGui::Button("R", buttonSize))
		{
			value = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();

		ImGui::Columns(1);
		ImGui::PopID();

		return valueChanged;
	}

	bool ImGuiUtils::DragFloat2(const std::string& label, glm::vec2& values, float resetValue, float speed, float min, float max)
	{
		bool valueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 4);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushMultiItemsWidths(2, ImGui::GetColumnWidth() - 2 * buttonSize.x);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.569f, 0.220f, 0.231f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &values.x, speed, min, max, "%.2f"))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.137f, 0.431f, 0.220f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &values.y, speed, min, max, "%.2f"))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return valueChanged;
	}

	bool ImGuiUtils::DragFloat3(const std::string& label, glm::vec3& values, float resetValue, float speed, float min, float max)
	{
		bool valueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 4);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushMultiItemsWidths(3, ImGui::GetColumnWidth() - 3 * buttonSize.x);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.569f, 0.220f, 0.231f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &values.x, speed, min, max, "%.2f"))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.137f, 0.431f, 0.220f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &values.y, speed, min, max, "%.2f"))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.271f, 0.590f, 0.900f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.180f, 0.384f, 0.588f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.271f, 0.590f, 0.900f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &values.z, speed, min, max, "%.2f"))
		{
			valueChanged = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return valueChanged;
	}

	bool ImGuiUtils::ColorEdit4(const std::string& label, float& value)
	{
		bool valueChanged = false;

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 4);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		valueChanged = ImGui::ColorEdit4("##Color", &value);
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopID();

		return valueChanged;
	}

	bool ImGuiUtils::BeginCombo(const std::string& label, const char& value)
	{
		bool valueChanged = false;

		ImGui::PushID(label.c_str());

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 4);

		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		valueChanged = ImGui::BeginCombo("##X", &value);

		if (!valueChanged)
		{
			ImGui::PopItemWidth();
			ImGui::Columns(1);
			ImGui::PopID();
		}

		return valueChanged;
	}

	void ImGuiUtils::EndCombo()
	{
		ImGui::EndCombo();

		ImGui::PopItemWidth();
		ImGui::Columns(1);
		ImGui::PopID();

	}

	void ImGuiUtils::BeginTextureViewer(const Ref<Texture2D> texture, float desiredWidth, float desiredHeight, bool flipped)
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

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 4);

		ImGui::Text("Texture");
		ImGui::NextColumn();

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

					float flippedY = desiredHeight - regionSize - regionY;

					if (flipped)
					{
						ImGui::Text("Max: (%d, %d)", (int)((regionX + regionSize) * resizeFactorWidth), (int)((flippedY + regionSize) * resizeFactorHeight));
						ImGui::Text("Min: (%d, %d)", (int)(regionX * resizeFactorWidth), (int)(flippedY * resizeFactorHeight));
					}
					else
					{
						ImGui::Text("Max: (%d, %d)", (int)((regionX + regionSize) * resizeFactorWidth), (int)((flippedY + regionSize) * resizeFactorHeight));
						ImGui::Text("Min: (%d, %d)", (int)(regionX * resizeFactorWidth), (int)(flippedY * resizeFactorHeight));
					}

					ImVec2 uv0;
					ImVec2 uv1;
					if (flipped)
					{
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

	bool ImGuiUtils::EndTextureViewer(const Ref<Texture2D> texture)
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
			ImGui::Text(texture->GetPath().stem().string().c_str());

			ImGui::SetCursorPosX(posX + padding);
			ImGui::SetCursorPosY(posY + ImGui::GetTextLineHeightWithSpacing());
			ImGui::Text("\n%dx%d", texture->GetWidth(), texture->GetHeight());

			ImGui::SetCursorPosY(nextY);
		}

		ImGui::Columns(1);

		return deleteTexture;
	}
}