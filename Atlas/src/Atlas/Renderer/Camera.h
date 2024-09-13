#pragma once

#include <glm/glm.hpp>

namespace Atlas
{
	class Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};

		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
	
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}