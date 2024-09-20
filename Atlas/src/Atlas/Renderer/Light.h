#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Light
	{
	public:
		Light() = default;
		Light(const glm::vec3& color, const float& strength)
			: m_Color(color), m_Intensity(strength) {}

		virtual ~Light() = default;

		void SetColor(const glm::vec3& color) { m_Color = color; }
		const glm::vec3& GetColor() { return m_Color; }

		void SetIntensity(const float& intensity) { m_Intensity = intensity; }
		const float& GetIntensity() { return m_Intensity; }

	protected:
		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;
	};
}