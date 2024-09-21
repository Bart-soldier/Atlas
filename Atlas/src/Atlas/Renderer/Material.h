#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Material
	{
	public:
		Material() = default;
		Material(const glm::vec4& color, const glm::vec3& ambientTint, const glm::vec3& diffuseTint, const glm::vec3& specularTint, const float& shininess)
			: m_Color(color), m_AmbientTint(ambientTint), m_DiffuseTint(diffuseTint), m_SpecularTint(specularTint), m_Shininess(shininess) {}

		virtual ~Material() = default;

		void SetColor(const glm::vec4& color) { m_Color = color; }
		const glm::vec4& GetColor() { return m_Color; }
		void SetAmbientTint(const glm::vec3& ambientTint) { m_AmbientTint = ambientTint; }
		const glm::vec3& GetAmbientTint() { return m_AmbientTint; }
		void SetDiffuseTint(const glm::vec3& diffuseTint) { m_DiffuseTint = diffuseTint; }
		const glm::vec3& GetDiffuseTint() { return m_DiffuseTint; }
		void SetSpecularTint(const glm::vec3& specularTint) { m_SpecularTint = specularTint; }
		const glm::vec3& GetSpecularTint() { return m_SpecularTint; }

		void SetShininess(const float& shininess) { m_Shininess = shininess; }
		const float& GetShininess() { return m_Shininess; }

	protected:
		glm::vec4 m_Color        = glm::vec4(1.0f);
		glm::vec3 m_AmbientTint  = glm::vec3(1.0f);
		glm::vec3 m_DiffuseTint  = glm::vec3(1.0f);
		glm::vec3 m_SpecularTint = glm::vec3(1.0f);
		float m_Shininess         = 0.25f;
	};
}