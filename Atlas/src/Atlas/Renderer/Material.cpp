#include "atlaspch.h"
#include "Atlas/Renderer/Material.h"

namespace Atlas
{
	Material::Material()
	{
		SetMaterialPreset(MaterialPresets::Default);
	}

	Material::Material(const MaterialPresets& materialPreset)
	{
		SetMaterialPreset(materialPreset);
	}

	Material::Material(const glm::vec4& color, const float& metallic, const float& roughness)
		: m_Color(color), m_Metallic(metallic), m_Roughness(roughness)
	{
		m_MaterialPreset = MaterialPresets::Custom;
	}

	Material::Material(const Ref<Texture2D>& albedoTexture,    const Ref<Texture2D>& normalTexture, const Ref<Texture2D>& metallicTexture,
		               const Ref<Texture2D>& roughnessTexture, const Ref<Texture2D>& aoTexture,     const Ref<Texture2D>& displacementTexture)
		: m_AlbedoTexture(albedoTexture),       m_NormalTexture(normalTexture), m_MetallicTexture(metallicTexture),
		  m_RoughnessTexture(roughnessTexture), m_AOTexture(aoTexture),         m_DisplacementTexture(displacementTexture)
	{
		m_Color = glm::vec3(1.0f);
		m_MaterialPreset = MaterialPresets::Custom;
	}

	void Material::SetAlbedoTexture(const Ref<Texture2D>& albedoTexture)
	{
		m_AlbedoTexture = albedoTexture;
		m_MaterialPreset = MaterialPresets::Custom;
		m_Color = glm::vec3(1.0f);
	}

	void Material::SetNormalTexture(const Ref<Texture2D>& normalTexture)
	{
		m_NormalTexture = normalTexture;
		m_MaterialPreset = MaterialPresets::Custom;
	}

	void Material::SetMetallicTexture(const Ref<Texture2D>& metallicTexture)
	{
		m_MetallicTexture = metallicTexture;
		m_MaterialPreset = MaterialPresets::Custom;
	}

	void Material::SetRoughnessTexture(const Ref<Texture2D>& roughnessTexture)
	{
		m_RoughnessTexture = roughnessTexture;
		m_MaterialPreset = MaterialPresets::Custom;
	}

	void Material::SetAOTexture(const Ref<Texture2D>& aoTexture)
	{
		m_AOTexture = aoTexture;
		m_MaterialPreset = MaterialPresets::Custom;
	}

	void Material::SetDisplacementTexture(const Ref<Texture2D>& displacementTexture)
	{
		m_DisplacementTexture = displacementTexture;
		m_MaterialPreset = MaterialPresets::Custom;
	}

	void Material::SetMaterialPreset(const MaterialPresets& materialPreset)
	{
		if (m_MaterialPreset == materialPreset)
		{
			return;
		}

		m_MaterialPreset = materialPreset;

		// These numbers come from the OpenGL teapots.c demo, Silicon Graphics, Inc., 1994, Mark J. Kilgard
		//switch (m_MaterialPreset)
		//{
		//	default:
		//	case Atlas::Material::MaterialPresets::Custom:
		//		return;
		//	case Atlas::Material::MaterialPresets::Default:
		//		m_Color  = glm::vec3(1.0f       , 1.0f       , 1.0f       );
		//		m_DiffuseColor  = glm::vec3(1.0f       , 1.0f       , 1.0f       );
		//		m_SpecularColor = glm::vec3(1.0f       , 1.0f       , 1.0f       );
		//		m_Metallic    = 0.25f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Emerald:
		//		m_Color  = glm::vec3(0.0215f    , 0.1745f    , 0.0215f    );
		//		m_DiffuseColor  = glm::vec3(0.07568f   , 0.61424f   , 0.07568f   );
		//		m_SpecularColor = glm::vec3(0.633f     , 0.727811f  , 0.633f     );
		//		m_Metallic    = 0.6f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Jade:
		//		m_Color  = glm::vec3(0.135f     , 0.2225f    , 0.1575f    );
		//		m_DiffuseColor  = glm::vec3(0.54f      , 0.89f      , 0.63f      );
		//		m_SpecularColor = glm::vec3(0.316228f  , 0.316228f  , 0.316228f  );
		//		m_Metallic    = 0.1f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Obsidian:
		//		m_Color  = glm::vec3(0.05375f   , 0.05f      , 0.06625f   );
		//		m_DiffuseColor  = glm::vec3(0.18275f   , 0.17f      , 0.22525f   );
		//		m_SpecularColor = glm::vec3(0.332741f  , 0.328634f  , 0.346435f  );
		//		m_Metallic    = 0.3f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Pearl:
		//		m_Color  = glm::vec3(0.25f      , 0.20725f   , 0.20725f   );
		//		m_DiffuseColor  = glm::vec3(1.0f       , 0.829f     , 0.829f     );
		//		m_SpecularColor = glm::vec3(0.296648f  , 0.296648f  , 0.296648f  );
		//		m_Metallic    = 0.088f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Ruby:
		//		m_Color  = glm::vec3(0.1745f    , 0.01175f   , 0.01175f   );
		//		m_DiffuseColor  = glm::vec3(0.61424f   , 0.04136f   , 0.04136f   );
		//		m_SpecularColor = glm::vec3(0.727811f  , 0.626959f  , 0.626959f  );
		//		m_Metallic    = 0.6f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Turquoise:
		//		m_Color  = glm::vec3(0.1f       , 0.18725f   , 0.1745f    );
		//		m_DiffuseColor  = glm::vec3(0.396f     , 0.74151f   , 0.69102f   );
		//		m_SpecularColor = glm::vec3(0.297254f  , 0.30829f   , 0.306678f  );
		//		m_Metallic    = 0.1f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Brass:
		//		m_Color  = glm::vec3(0.329412f  , 0.223529f  , 0.027451f  );
		//		m_DiffuseColor  = glm::vec3(0.780392f  , 0.568627f  , 0.113725f  );
		//		m_SpecularColor = glm::vec3(0.992157f  , 0.941176f  , 0.807843f  );
		//		m_Metallic    = 0.21794872f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Bronze:
		//		m_Color  = glm::vec3(0.2125f    , 0.1275f    , 0.054f     );
		//		m_DiffuseColor  = glm::vec3(0.714f     , 0.4284f    , 0.18144f   );
		//		m_SpecularColor = glm::vec3(0.393548f  , 0.271906f  , 0.166721f  );
		//		m_Metallic    = 0.2f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Chrome:
		//		m_Color  = glm::vec3(0.25f      , 0.25f      , 0.25f      );
		//		m_DiffuseColor  = glm::vec3(0.4f       , 0.4f       , 0.4f       );
		//		m_SpecularColor = glm::vec3(0.774597f  , 0.774597f  , 0.774597f  );
		//		m_Metallic    = 0.6f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Copper:
		//		m_Color  = glm::vec3(0.19125f   , 0.0735f    , 0.0225f    );
		//		m_DiffuseColor  = glm::vec3(0.7038f    , 0.27048f   , 0.0828f    );
		//		m_SpecularColor = glm::vec3(0.256777f  , 0.137622f  , 0.086014f  );
		//		m_Metallic    = 0.1f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Gold:
		//		m_Color  = glm::vec3(0.24725f   , 0.1995f    , 0.0745f    );
		//		m_DiffuseColor  = glm::vec3(0.75164f   , 0.60648f   , 0.22648f   );
		//		m_SpecularColor = glm::vec3(0.628281f  , 0.555802f  , 0.366065f  );
		//		m_Metallic    = 0.4f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Silver:
		//		m_Color  = glm::vec3(0.19225f   , 0.19225f   , 0.19225f   );
		//		m_DiffuseColor  = glm::vec3(0.50754f   , 0.50754f   , 0.50754f   );
		//		m_SpecularColor = glm::vec3(0.508273f  , 0.508273f  , 0.508273f  );
		//		m_Metallic    = 0.4f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Plastic_Black:
		//		m_Color  = glm::vec3(0.0f       , 0.0f       , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.01f      , 0.01f      , 0.01f      );
		//		m_SpecularColor = glm::vec3(0.50f      , 0.50f      , 0.50f      );
		//		m_Metallic    = 0.25f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Plastic_Cyan:
		//		m_Color  = glm::vec3(0.0f       , 0.1f       , 0.06f      );
		//		m_DiffuseColor  = glm::vec3(0.0f       , 0.50980392f, 0.50980392f);
		//		m_SpecularColor = glm::vec3(0.50196078f, 0.50196078f, 0.50196078f);
		//		m_Metallic    = 0.25f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Plastic_Green:
		//		m_Color  = glm::vec3(0.0f       , 0.0f       , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.1f       , 0.35f      , 0.1f       );
		//		m_SpecularColor = glm::vec3(0.45f      , 0.55f      , 0.45f      );
		//		m_Metallic    = 0.25f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Plastic_Red:
		//		m_Color  = glm::vec3(0.0f       , 0.0f       , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.5f       , 0.0f       , 0.0f       );
		//		m_SpecularColor = glm::vec3(0.7f       , 0.6f       , 0.6f       );
		//		m_Metallic    = 0.25f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Plastic_White:
		//		m_Color  = glm::vec3(0.0f       , 0.0f       , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.55f      , 0.55f      , 0.55f      );
		//		m_SpecularColor = glm::vec3(0.70f      , 0.70f      , 0.70f      );
		//		m_Metallic    = 0.25f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Plastic_Yellow:
		//		m_Color  = glm::vec3(0.0f       , 0.0f       , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.5f       , 0.5f       , 0.0f       );
		//		m_SpecularColor = glm::vec3(0.60f      , 0.60f      , 0.50f      );
		//		m_Metallic    = 0.25f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Rubber_Black:
		//		m_Color  = glm::vec3(0.02f      , 0.02f      , 0.02f      );
		//		m_DiffuseColor  = glm::vec3(0.01f      , 0.01f      , 0.01f      );
		//		m_SpecularColor = glm::vec3(0.4f       , 0.4f       , 0.4f       );
		//		m_Metallic    = 0.078125f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Rubber_Cyan:
		//		m_Color  = glm::vec3(0.0f       , 0.05f      , 0.05f      );
		//		m_DiffuseColor  = glm::vec3(0.4f       , 0.5f       , 0.5f       );
		//		m_SpecularColor = glm::vec3(0.04f      , 0.7f       , 0.7f       );
		//		m_Metallic    = 0.078125f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Rubber_Green:
		//		m_Color  = glm::vec3(0.0f       , 0.05f      , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.4f       , 0.5f       , 0.4f       );
		//		m_SpecularColor = glm::vec3(0.04f      , 0.7f       , 0.04f      );
		//		m_Metallic    = 0.078125f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Rubber_Red:
		//		m_Color  = glm::vec3(0.05f      , 0.0f       , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.5f       , 0.4f       , 0.4f       );
		//		m_SpecularColor = glm::vec3(0.7f       , 0.04f      , 0.04f      );
		//		m_Metallic    = 0.078125f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Rubber_White:
		//		m_Color  = glm::vec3(0.05f      , 0.05f      , 0.05f      );
		//		m_DiffuseColor  = glm::vec3(0.5f       , 0.5f       , 0.5f       );
		//		m_SpecularColor = glm::vec3(0.7f       , 0.7f       , 0.7f       );
		//		m_Metallic    = 0.078125f;
		//		break;
		//	case Atlas::Material::MaterialPresets::Rubber_Yellow:
		//		m_Color  = glm::vec3(0.05f      , 0.05f      , 0.0f       );
		//		m_DiffuseColor  = glm::vec3(0.5f       , 0.5f       , 0.4f       );
		//		m_SpecularColor = glm::vec3(0.7f       , 0.7f       , 0.04f      );
		//		m_Metallic    = 0.078125f;
		//		break;
		//}

		m_AlbedoTexture       = nullptr;
		m_NormalTexture       = nullptr;
		m_MetallicTexture     = nullptr;
		m_RoughnessTexture    = nullptr;
		m_AOTexture           = nullptr;
		m_DisplacementTexture = nullptr;
	}
}