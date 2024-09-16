#include "Panels/ContentBrowserPanel.h"

#include "Atlas/Project/Project.h"

#include <imgui/imgui.h>

namespace Atlas
{
	ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		m_AtlasIcon = Texture2D::Create("Resources/Icons/ContentBrowser/AtlasIcon.png");
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
		m_ImageIcon = Texture2D::Create("Resources/Icons/ContentBrowser/ImageIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_BaseDirectory.empty())
		{
			ImGui::End();
			return;
		}

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 185.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			Ref<Texture2D> icon = GetDirectoryEntryIcon(directoryEntry);
			ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		// TODO: status bar
		ImGui::End();
	}

	Ref<Texture2D> ContentBrowserPanel::GetDirectoryEntryIcon(const std::filesystem::directory_entry directoryEntry)
	{
		if (directoryEntry.is_directory())
		{
			return m_DirectoryIcon;
		}

		std::string extension = directoryEntry.path().extension().string();

		if (extension == ".atlas")
		{
			return m_AtlasIcon;
		}
		if (extension == ".png" || extension == ".jpeg")
		{
			return m_ImageIcon;
		}
		else
		{
			return m_FileIcon;
		}
	}
}