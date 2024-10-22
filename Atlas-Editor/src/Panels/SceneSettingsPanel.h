#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Scene/Scene.h"

namespace Atlas
{
	class SceneSettingsPanel
	{
	public:
		SceneSettingsPanel() = default;
		SceneSettingsPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

	private:
		void DrawPrimaryCamera();
		void DrawSkybox();

		Ref<Scene> m_Context;
	};
}
