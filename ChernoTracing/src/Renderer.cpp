#include "Renderer.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"

/*****************************************************************************/
// c_Renderer

/*===========================================================================*/
void c_Renderer::Render(Walnut::Image& r_Image)
{
	const int Width = r_Image.GetWidth();
	const int Height = r_Image.GetHeight();

	m_ImageData.resize(Width * Height);

	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			const int PixelIndex = x + y * Width;

			const glm::vec2 PixelUV
				( static_cast<float>(x) / Width
				, 1.0f - static_cast<float>(y) / Height
				);

			m_ImageData[PixelIndex] = RenderPixel(PixelUV);
		}
	}

	r_Image.SetData(m_ImageData.data());
}

/*===========================================================================*/
s_RGBA c_Renderer::RenderPixel(const glm::vec2& PixelUV)
{
	const glm::vec3 RayOrigin(0.0f, 0.0f, 2.0f);
	const glm::vec3
		RayDirection(PixelUV.x * 2.0f - 1.0f, PixelUV.y * 2.0f - 1.0f, 1.0f);

	// Sphere
	bool HitSphere = false;
	{
		constexpr float Radius = 0.5f;

		const float a = glm::dot(RayDirection, RayDirection);
		const float b = 2.0f * glm::dot(RayOrigin, RayDirection);
		const float c = glm::dot(RayOrigin, RayOrigin) - Radius * Radius;

		const float Discriminant = b * b - 4.0f * a * c;

		HitSphere = (Discriminant >= 0.0f);
	}

	s_RGBA Result;
	if (HitSphere)
	{
		Result.Channel.R = 0;
		Result.Channel.G = 0;
		Result.Channel.B = 255;
	}
	else
	{
		Result.Channel.R = static_cast<uint8_t>(PixelUV.x * 255);
		Result.Channel.G = static_cast<uint8_t>(PixelUV.y * 255);
		Result.Channel.B = 0;
	}
	Result.Channel.A = 255;

	return Result;
}

/*===========================================================================*/
