#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Renderer/Texture.h"

#include <string>

namespace Atlas
{
	class ImGuiUtils
	{
	public:
		static bool DragFloat(const std::string& label, float& value, float speed, float min, float max, float resetValue);
		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f);
		static void DrawTextureViewerPreDragDropTarget(const Ref<Texture2D> texture, float desiredWidth, float desiredHeight, bool flipped);
		static bool DrawTextureViewerPostDragDropTarget(const Ref<Texture2D> texture);
	};
}