#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Light
	{
	public:
		enum class CastType
		{
			DirectionalLight = 0,
			PointLight       = 1,
			Spotlight        = 2,
		};

		Light() = default;
		Light(const glm::vec3& color, const float& intensity)
			: m_Color(color), m_Intensity(intensity) {}

		virtual ~Light() = default;

		void SetColor(const glm::vec3& color) { m_Color = color; }
		const glm::vec3& GetColor() { return m_Color; }
		void SetDirection(const glm::vec3& direction) { m_Direction = glm::vec4(direction, m_Direction.w); }
		const glm::vec4& GetDirection() { return m_Direction; }
		void SetAttenuation(const glm::vec3& attenuation) { m_Attenuation = glm::vec4(attenuation, m_Attenuation.w); }
		const glm::vec4& GetAttenuation() { return m_Attenuation; }
		void SetIntensity(const float& intensity) { m_Intensity = intensity; }
		const float& GetIntensity() { return m_Intensity; }

		void SetAmbientStrength(const float& ambientStrength) { m_AmbientStrength = ambientStrength; }
		const float& GetAmbientStrength() { return m_AmbientStrength; }
		void SetDiffuseStrength(const float& diffuseStrength) { m_DiffuseStrength = diffuseStrength; }
		const float& GetDiffuseStrength() { return m_DiffuseStrength; }
		void SetSpecularStrength(const float& specularStrength) { m_SpecularStrength = specularStrength; }
		const float& GetSpecularStrength() { return m_SpecularStrength; }

		CastType GetCastType() const { return m_CastType; }
		void SetCastType(CastType castType);

	protected:
		CastType m_CastType = CastType::PointLight;

		glm::vec3 m_Color = glm::vec3(1.0f);
		glm::vec4 m_Direction = glm::vec4(0.0f); // w is used as flag to signal if we use direction or not
		glm::vec4 m_Attenuation = glm::vec4(1.0f, 0.09f, 0.032f, 1.0f); // w is used as flag to signal if we use direction or not
		float m_Intensity = 1.0f;

		float m_AmbientStrength  = 0.1f;
		float m_DiffuseStrength  = 0.5f;
		float m_SpecularStrength = 1.0f;
	};
}