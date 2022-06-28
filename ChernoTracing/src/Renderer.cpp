#include "Renderer.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"

/*****************************************************************************/
// c_Renderer

/*===========================================================================*/
void c_Renderer::Render(Walnut::Image& r_Image)
{
	m_ImageData.resize(r_Image.GetWidth() * r_Image.GetHeight());

	for (s_RGBA& r_Pixel : m_ImageData)
		r_Pixel = RenderPixel();

	r_Image.SetData(m_ImageData.data());
}

/*===========================================================================*/
s_RGBA c_Renderer::RenderPixel()
{
	return s_RGBA{.Value = (Walnut::Random::UInt() | 0xFF000000)};
}

/*===========================================================================*/
