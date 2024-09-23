#include "atlaspch.h"
#include "Atlas/Renderer/Light.h"

namespace Atlas
{
	void Light::SetCastType(CastType castType)
	{
		m_CastType = castType;

		if (m_CastType == CastType::PointLight)
		{
			m_Direction.w = 0.0f;
		}
		else
		{
			m_Direction.w = 1.0f;
		}
	}
}