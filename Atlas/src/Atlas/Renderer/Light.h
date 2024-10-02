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
		void SetRadius(const float& radius) { m_Radius = radius; }
		const float& GetRadius() { return m_Radius; }
		void SetIntensity(const float& intensity) { m_Intensity = intensity; }
		const float& GetIntensity() { return m_Intensity; }
		void SetCutOff(const glm::vec2& cutOff) { m_CutOff = cutOff; }
		const glm::vec2& GetCutOff() { return m_CutOff; }

		void SetAmbientStrength(const float& ambientStrength) { m_AmbientStrength = ambientStrength; }
		const float& GetAmbientStrength() { return m_AmbientStrength; }
		void SetDiffuseStrength(const float& diffuseStrength) { m_DiffuseStrength = diffuseStrength; }
		const float& GetDiffuseStrength() { return m_DiffuseStrength; }
		void SetSpecularStrength(const float& specularStrength) { m_SpecularStrength = specularStrength; }
		const float& GetSpecularStrength() { return m_SpecularStrength; }

		void SetCastType(const CastType& castType);
		const CastType& GetCastType() { return m_CastType; }

	protected:
		CastType m_CastType = CastType::PointLight;

		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Radius = 0.01f; // Negative means attenuation disabled
		float m_Intensity = 20.0f;
		glm::vec2 m_CutOff = -glm::vec2(12.5, 17.5); // (inner, outer), negative means cutoff disabled

		float m_AmbientStrength  = 0.1f;
		float m_DiffuseStrength  = 0.5f;
		float m_SpecularStrength = 1.0f;
	};
}