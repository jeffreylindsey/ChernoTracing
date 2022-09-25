#pragma once

#include "Color.h"
#include "Ray.h"
#include "Sphere.h"

#include <glm/glm.hpp>

#include <optional>
#include <vector>

class c_Camera;

namespace Walnut
{
	class Image;
}

/*****************************************************************************/
// c_Renderer

class c_Renderer
{
	//--- Public Methods ------------------------------------------------------
	public:
		void Render(Walnut::Image& r_Image, const c_Camera& Camera);

	//--- Private Methods -----------------------------------------------------
	private:
		glm::vec4 RenderPixel(const s_Ray& Ray);

		std::optional<glm::vec3> RenderSphere
			( const s_Ray& Ray
			, const glm::vec3& LightDirection
			) const;
		std::optional<float> HitSphere
			( const s_Ray& Ray
			, const s_Sphere& Sphere
			) const;

	//--- Private Members -----------------------------------------------------
	private:
		std::vector<s_RGBA> m_ImageData;
};

/*****************************************************************************/
