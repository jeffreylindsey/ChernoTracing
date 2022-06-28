#pragma once

#include "Color.h"

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
		s_RGBA RenderPixel();

	//--- Private Members -----------------------------------------------------
	private:
		std::vector<s_RGBA> m_ImageData;
};

/*****************************************************************************/
