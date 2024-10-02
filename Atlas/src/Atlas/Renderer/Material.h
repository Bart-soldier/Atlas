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
			Default        =  1,
			Emerald        =  2,
			Jade           =  3,
			Obsidian       =  4,
			Pearl          =  5,
			Ruby           =  6,
			Turquoise      =  7,
			Brass          =  8,
			Bronze         =  9,
			Chrome         = 10,
			Copper         = 11,
			Gold           = 12,
			Silver         = 13,
			Plastic_Black  = 14,
			Plastic_Cyan   = 15,
			Plastic_Green  = 16,
			Plastic_Red    = 17,
			Plastic_White  = 18,
			Plastic_Yellow = 19,
			Rubber_Black   = 20,
			Rubber_Cyan    = 21,
			Rubber_Green   = 22,
			Rubber_Red     = 23,
			Rubber_White   = 24,
			Rubber_Yellow  = 25,
		};

		Material() { SetMaterialPreset(MaterialPresets::Default); }
		Material(const MaterialPresets& materialPreset) { SetMaterialPreset(materialPreset); }
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

		void SetSpecularTexture(const Ref<Texture2D>& specularTexture);
		const Ref<Texture2D>& GetSpecularTexture() { return m_SpecularTexture; }

		void SetShininess(const float& shininess) { m_Shininess = shininess; m_MaterialPreset = MaterialPresets::Custom; }
		const float& GetShininess() { return m_Shininess; }

		void SetMaterialPreset(const MaterialPresets& materialPreset);
		const MaterialPresets& GetMaterialPreset() { return m_MaterialPreset; }

	private:
		MaterialPresets m_MaterialPreset = MaterialPresets::Custom;

		glm::vec3 m_AmbientColor;
		glm::vec3 m_DiffuseColor;
		glm::vec3 m_SpecularColor;
		float m_Shininess;

		Ref<Texture2D> m_DiffuseTexture;
		Ref<Texture2D> m_SpecularTexture;
	};
}