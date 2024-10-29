#include "atlaspch.h"
#include "Atlas/Renderer/Light.h"

namespace Atlas
{
	void Light::SetCastType(const CastType& castType)
	{
		m_CastType = castType;

		switch (castType)
		{
			case CastType::DirectionalLight:
				m_CutOff = -abs(m_CutOff);
				break;
			case CastType::PointLight:
				m_CutOff = -abs(m_CutOff);
				break;
			case CastType::Spotlight:
				m_CutOff = abs(m_CutOff);
				break;
		}
	}
}