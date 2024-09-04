#pragma once

#include <glm/glm.hpp>

#include "Atlas/Core/KeyCodes.h"
#include "Atlas/Core/MouseCodes.h"

namespace Atlas
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}