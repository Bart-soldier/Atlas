#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Light
	{
	public:
		Light() = default;
		Light(const glm::vec3& color, const float& intensity)
			: m_Color(color), m_Intensity(intensity) {}

		virtual ~Light() = default;

		void SetColor(const glm::vec3& color) { m_Color = color; }
		const glm::vec3& GetColor() { return m_Color; }
		void SetIntensity(const float& intensity) { m_Intensity = intensity; }
		const float& GetIntensity() { return m_Intensity; }

		void SetAmbientStrength(const float& ambientStrength) { m_AmbientStrength = ambientStrength; }
		const float& GetAmbientStrength() { return m_AmbientStrength; }
		void SetDiffuseStrength(const float& diffuseStrength) { m_DiffuseStrength = diffuseStrength; }
		const float& GetDiffuseStrength() { return m_DiffuseStrength; }
		void SetSpecularStrength(const float& specularStrength) { m_SpecularStrength = specularStrength; }
		const float& GetSpecularStrength() { return m_SpecularStrength; }

	protected:
		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;

		float m_AmbientStrength  = 0.1f;
		float m_DiffuseStrength  = 0.5f;
		float m_SpecularStrength = 1.0f;
	};
}