#include "Renderer.h"

#include "Walnut/Random.h"

/*****************************************************************************/
// c_Renderer

/*===========================================================================*/
void c_Renderer::Render(std::vector<s_RGBA>& r_ImageData)
{
	for (s_RGBA& r_Pixel : r_ImageData)
		r_Pixel.Value = Walnut::Random::UInt() | 0xFF000000;
}

/*===========================================================================*/
