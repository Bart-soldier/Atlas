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
		void SetIntensity(const float& intensity) { m_Intensity = intensity; }
		const float& GetIntensity() { return m_Intensity; }
		void SetCutOff(const glm::vec2& cutOff) { m_CutOff = cutOff; }
		const glm::vec2& GetCutOff() { return m_CutOff; }

		void SetCastType(const CastType& castType);
		const CastType& GetCastType() { return m_CastType; }

	protected:
		CastType m_CastType = CastType::PointLight;

		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 20.0f;
		glm::vec2 m_CutOff = -glm::vec2(12.5, 17.5); // (inner, outer), negative means cutoff disabled
	};
}