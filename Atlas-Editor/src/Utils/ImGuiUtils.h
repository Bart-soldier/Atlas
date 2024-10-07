#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Renderer/Texture.h"

#include <string>

namespace Atlas
{
	class ImGuiUtils
	{
	public:
		static bool Checkbox(const std::string& label, bool& value);
		static bool DragFloat(const std::string& label, float& value, float resetValue = 0.0f, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat2(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat3(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool ColorEdit3(const std::string& label, float& value);
		static bool ColorEdit4(const std::string& label, float& value);

		static bool BeginCombo(const std::string& label, const char& value, float maxWidth = -1, const int& uniqueID = -1);
		static void EndCombo();

		static void BeginTextureViewer(const std::string& label, const Ref<Texture2D> texture, float desiredWidth, float desiredHeight, bool flipped);
		static bool EndTextureViewer(const Ref<Texture2D> texture);
	};
}