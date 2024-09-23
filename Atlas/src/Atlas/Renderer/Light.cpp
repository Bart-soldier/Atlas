#include "atlaspch.h"
#include "Atlas/Renderer/Light.h"

namespace Atlas
{
	void Light::SetCastType(CastType castType)
	{
		m_CastType = castType;

		switch (castType)
		{
			case CastType::DirectionalLight:
				m_Direction.w   = 1.0f;
				m_Radius = -abs(m_Radius);
				break;
			case CastType::PointLight:
				m_Direction.w   = 0.0f;
				m_Radius = abs(m_Radius);
				break;
			case CastType::Spotlight:
				m_Direction.w   = 1.0f;
				m_Radius = abs(m_Radius);
				break;
		}
	}
}