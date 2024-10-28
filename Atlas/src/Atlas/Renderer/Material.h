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

		Material();
		Material(const MaterialPresets& materialPreset);
		Material(const glm::vec4& color, const float& metallic, const float& roughness);
		Material(const Ref<Texture2D>& albedoTexture,    const Ref<Texture2D>& normalTexture, const Ref<Texture2D>& metallicTexture,
			     const Ref<Texture2D>& roughnessTexture, const Ref<Texture2D>& aoTexture,     const Ref<Texture2D>& displacementTexture);

		virtual ~Material() = default;

		// Material Properties
		void SetColor(const glm::vec3& ambientTint) { m_Color = ambientTint; m_MaterialPreset = MaterialPresets::Custom; }
		const glm::vec3& GetColor() { return m_Color; }
		void SetMetallic(const float& metallic) { m_Metallic = metallic; m_MaterialPreset = MaterialPresets::Custom; }
		const float& GetMetallic() { return m_Metallic; }
		void SetRoughness(const float& roughness) { m_Roughness = roughness; m_MaterialPreset = MaterialPresets::Custom; }
		const float& GetRoughness() { return m_Roughness; }

		// Material Textures
		void SetAlbedoTexture(const Ref<Texture2D>& albedoTexture);
		const Ref<Texture2D>& GetAlbedoTexture() { return m_AlbedoTexture; }
		void SetNormalTexture(const Ref<Texture2D>& normalTexture);
		const Ref<Texture2D>& GetNormalTexture() { return m_NormalTexture; }
		void SetMetallicTexture(const Ref<Texture2D>& metallicTexture);
		const Ref<Texture2D>& GetMetallicTexture() { return m_MetallicTexture; }
		void SetRoughnessTexture(const Ref<Texture2D>& roughnessTexture);
		const Ref<Texture2D>& GetRoughnessTexture() { return m_RoughnessTexture; }
		void SetAOTexture(const Ref<Texture2D>& aoTexture);
		const Ref<Texture2D>& GetAOTexture() { return m_AOTexture; }
		void SetDisplacementTexture(const Ref<Texture2D>& displacementTexture);
		const Ref<Texture2D>& GetDisplacementTexture() { return m_DisplacementTexture; }

		void SetMaterialPreset(const MaterialPresets& materialPreset);
		const MaterialPresets& GetMaterialPreset() { return m_MaterialPreset; }

	private:
		MaterialPresets m_MaterialPreset = MaterialPresets::Default;

		glm::vec3 m_Color = glm::vec3(1.0f, 1.0f, 1.0f);
		float m_Metallic = 0.25f;
		float m_Roughness = 0.25f;

		Ref<Texture2D> m_AlbedoTexture       = nullptr;
		Ref<Texture2D> m_NormalTexture       = nullptr;
		Ref<Texture2D> m_MetallicTexture     = nullptr;
		Ref<Texture2D> m_RoughnessTexture    = nullptr;
		Ref<Texture2D> m_AOTexture           = nullptr;
		Ref<Texture2D> m_DisplacementTexture = nullptr;
	};
}