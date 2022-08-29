#pragma once

#include "Color.h"

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
		glm::vec4 RenderPixel
			( const glm::vec3& RayOrigin
			, const glm::vec3& RayDirection
			);

		std::optional<glm::vec3> RenderSphere
			( const glm::vec3& RayOrigin
			, const glm::vec3& RayDirection
			, const glm::vec3& LightDirection
			) const;

	//--- Private Members -----------------------------------------------------
	private:
		std::vector<s_RGBA> m_ImageData;
};

/*****************************************************************************/
