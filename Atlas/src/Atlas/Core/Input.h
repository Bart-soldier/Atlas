#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Core/KeyCodes.h"
#include "Atlas/Core/MouseCodes.h"

namespace Atlas
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		
		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}