#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Renderer/Texture.h"

#include <filesystem>

namespace Atlas
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		Ref<Texture2D> GetDirectoryEntryIcon(const std::filesystem::directory_entry directoryEntry);

		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_AtlasIcon;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_ImageIcon;
	};
}