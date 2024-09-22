#pragma once

#include <glm/glm.hpp>

#include "Atlas/Renderer/Texture.h"

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
		Material(const glm::vec4& color, const glm::vec3& ambientColor, const glm::vec3& diffuseColor, const glm::vec3& specularColor, const float& shininess)
			: m_AmbientColor(ambientColor), m_DiffuseColor(diffuseColor), m_SpecularColor(specularColor), m_Shininess(shininess) {}

		virtual ~Material() = default;

		void SetAmbientColor(const glm::vec3& ambientTint) { m_AmbientColor = ambientTint; m_MaterialPreset = MaterialPresets::Custom; }
		const glm::vec3& GetAmbientColor() { return m_AmbientColor; }
		void SetDiffuseColor(const glm::vec3& diffuseTint) { m_DiffuseColor = diffuseTint; m_MaterialPreset = MaterialPresets::Custom; }
		const glm::vec3& GetDiffuseColor() { return m_DiffuseColor; }
		void SetSpecularColor(const glm::vec3& specularTint) { m_SpecularColor = specularTint; m_MaterialPreset = MaterialPresets::Custom; }
		const glm::vec3& GetSpecularColor() { return m_SpecularColor; }

		void SetDiffuseTexture(const Ref<Texture2D>& diffuseTexture);
		const Ref<Texture2D>& GetDiffuseTexture() { return m_DiffuseTexture; }

		void SetShininess(const float& shininess) { m_Shininess = shininess; m_MaterialPreset = MaterialPresets::Custom; }
		const float& GetShininess() { return m_Shininess; }

		MaterialPresets GetMaterialPreset() const { return m_MaterialPreset; }
		void SetMaterialPreset(MaterialPresets materialPreset);

	protected:
		MaterialPresets m_MaterialPreset = MaterialPresets::Custom;

		glm::vec3 m_AmbientColor  = glm::vec3(1.0f);
		glm::vec3 m_DiffuseColor  = glm::vec3(1.0f);
		glm::vec3 m_SpecularColor = glm::vec3(1.0f);
		float m_Shininess         = 0.25f;

		Ref<Texture2D> m_DiffuseTexture = nullptr;
	};
}