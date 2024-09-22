#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Material
	{
	public:
		enum class MaterialPresets
		{
			Custom         =  0,
			Emerald        =  1,
			Jade           =  2,
			Obsidian       =  3,
			Pearl          =  4,
			Ruby           =  5,
			Turquoise      =  6,
			Brass          =  7,
			Bronze         =  8,
			Chrome         =  9,
			Copper         = 10,
			Gold           = 11,
			Silver         = 12,
			Plastic_Black  = 13,
			Plastic_Cyan   = 14,
			Plastic_Green  = 15,
			Plastic_Red    = 16,
			Plastic_White  = 17,
			Plastic_Yellow = 18,
			Rubber_Black   = 19,
			Rubber_Cyan    = 20,
			Rubber_Green   = 21,
			Rubber_Red     = 22,
			Rubber_White   = 23,
			Rubber_Yellow  = 24,
		};

		Material() = default;
		Material(const glm::vec4& color, const glm::vec3& ambientTint, const glm::vec3& diffuseTint, const glm::vec3& specularTint, const float& shininess)
			: m_Color(color), m_AmbientTint(ambientTint), m_DiffuseTint(diffuseTint), m_SpecularTint(specularTint), m_Shininess(shininess) {}

		virtual ~Material() = default;

		MaterialPresets GetMaterialPreset() const { return m_MaterialPreset; }
		void SetMaterialPreset(MaterialPresets materialPreset);

		void SetColor(const glm::vec4& color) { m_Color = color; }
		const glm::vec4& GetColor() { return m_Color; }
		void SetAmbientTint(const glm::vec3& ambientTint) { m_AmbientTint = ambientTint; m_MaterialPreset = MaterialPresets::Custom; }
		const glm::vec3& GetAmbientTint() { return m_AmbientTint; }
		void SetDiffuseTint(const glm::vec3& diffuseTint) { m_DiffuseTint = diffuseTint; m_MaterialPreset = MaterialPresets::Custom; }
		const glm::vec3& GetDiffuseTint() { return m_DiffuseTint; }
		void SetSpecularTint(const glm::vec3& specularTint) { m_SpecularTint = specularTint; m_MaterialPreset = MaterialPresets::Custom; }
		const glm::vec3& GetSpecularTint() { return m_SpecularTint; }

		void SetShininess(const float& shininess) { m_Shininess = shininess; m_MaterialPreset = MaterialPresets::Custom; }
		const float& GetShininess() { return m_Shininess; }

	protected:
		MaterialPresets m_MaterialPreset = MaterialPresets::Custom;

		glm::vec4 m_Color        = glm::vec4(1.0f);
		glm::vec3 m_AmbientTint  = glm::vec3(1.0f);
		glm::vec3 m_DiffuseTint  = glm::vec3(1.0f);
		glm::vec3 m_SpecularTint = glm::vec3(1.0f);
		float m_Shininess         = 0.25f;
	};
}