#pragma once

#include "Color.h"

#include <glm/glm.hpp>

#include <vector>

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
		void Render(Walnut::Image& r_Image);

	//--- Private Methods -----------------------------------------------------
	private:
		glm::vec4 RenderPixel(const glm::vec2& PixelUV);

	//--- Private Members -----------------------------------------------------
	private:
		std::vector<s_RGBA> m_ImageData;
};

/*****************************************************************************/
