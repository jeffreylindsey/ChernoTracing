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
	const glm::vec3 RayOrigin(0.0f, 0.0f, -2.0f);
	const glm::vec3
		RayDirection(PixelUV.x * 2.0f - 1.0f, PixelUV.y * 2.0f - 1.0f, 1.0f);

	// Ray: Point = Origin + Dirction*t, where t >= 0.
	// Sphere: Point = Origin + ()*Radius
	/*
	Radius = ((Point.x - Origin.x)^2 + (Point.y - Origin.y)^2 + (Point.z - Origin.z)^2)^(1/2)
	Radius^2 = (Point.x - Origin.x)^2 + (Point.y - Origin.y)^2 + (Point.z - Origin.z)^2
	Radius^2 = (P.x - O.x)^2 + (P.y - O.y)^2 + (P.z - O.z)^2
	Radius^2 = (P.x - O.x)(P.x - O.x) + (P.y - O.y)(P.y - O.y) + (P.z - O.z)(P.z - O.z)
	Radius^2 = ((P.x)(P.x) + (P.x)(-O.x) + (-O.x)(P.x) + (-O.x)(-O.x)) ...
	Radius^2 = ((P.x)^2 - 2(P.x)(O.x) + (O.x)^2) + ((P.y)^2 - 2(P.y)(O.y) + (O.y)^2) + ((P.z)^2 - 2(P.z)(O.z) + (O.z)^2)
	Radius^2 = (P.x)^2 + (P.y)^2 + (P.z)^2 - 2(P.x)(O.x) - 2(P.y)(O.y) - 2(P.z)(O.z) + (O.y)^2 + (O.x)^2 + (O.z)^2
	Radius^2 = P*P - 2(P*O) + O*O
	Sr^2 = P*P - 2(P*So) + So*So

	P = Ro + Rd * t

	Sr^2 = (Ro + Rd * t)*(Ro + Rd * t) - 2((Ro + Rd * t)*So) + So*So
	Sr^2 = ((Ro + Rd * t).x)^2 + ((Ro + Rd * t).y)^2 + ((Ro + Rd * t).z)^2 - 2((Ro + Rd * t).x)(So.x) - 2((Ro + Rd * t).y)(So.y) - 2((Ro + Rd * t).z)(So.z) + (So.x)^2 + (So.y)^2 + (So.z)^2
	Sr^2 = (Ro.x + Rd.x*t)^2 + (Ro.y + Rd.y*t)^2 + (Ro.z + Rd.z*t)^2 - 2(Ro.x + Rd.x*t)(So.x) - 2(Ro.y + Rd.y*t)(So.y) - 2(Ro.z + Rd.z*t)(So.z) + (So.x)^2 + (So.y)^2 + (So.z)^2
	Sr^2 = (Ro.x)^2 + 2(Ro.x)(Rd.x*t) + (Rd.x*t)^2 + (Ro.y)^2 + 2(Ro.y)(Rd.y*t) + (Rd.y*t)^2 + (Ro.z)^2 + 2(Ro.z)(Rd.z*t) + (Rd.z*t)^2 ...
	Sr^2 = (Ro.x)^2 + 2t(Ro.x)(Rd.x) + (t^2)(Rd.x)^2 + (Ro.y)^2 + 2t(Ro.y)(Rd.y) + (t^2)(Rd.y)^2 + (Ro.z)^2 + 2t(Ro.z)(Rd.z) + (t^2)(Rd.z) ...
	Sr^2 = (Ro.x)^2 + (Ro.y)^2 + (Ro.z)^2 + 2t(Ro.x)(Rd.x) + 2t(Ro.y)(Rd.y) + 2t(Ro.z)(Rd.z) + (t^2)(Rd.x)^2 + (t^2)(Rd.y)^2 + (t^2)(Rd.z) ...
	Sr^2 = (Ro.x)^2 + (Ro.y)^2 + (Ro.z)^2 + 2t((Ro.x)(Rd.x) + (Ro.y)(Rd.y) + (Ro.z)(Rd.z)) + (t^2)((Rd.x)^2 + (Rd.y)^2 + (Rd.z)) ...
	Sr^2 = Ro*Ro + 2t(Ro*Rd) + (t^2)(Rd*Rd) ...
	Sr^2 = ... - 2(Ro.x + Rd.x*t)(So.x) - 2(Ro.y + Rd.y*t)(So.y) - 2(Ro.z + Rd.z*t)(So.z) ...
	Sr^2 = ... - 2((Ro.x + Rd.x*t)(So.x) + (Ro.y + Rd.y*t)(So.y) + (Ro.z + Rd.z*t)(So.z)) ...
	Sr^2 = ... - 2((Ro.x)(So.x) + (Rd.x*t)(So.x) + (Ro.y)(So.y) + (Rd.y*t)(So.y) + (Ro.z)(So.z) + (Rd.z*t)(So.z)) ...
	Sr^2 = ... - 2((Ro.x)(So.x) + t(Rd.x)(So.x) + (Ro.y)(So.y) + t(Rd.y)(So.y) + (Ro.z)(So.z) + t(Rd.z)(So.z)) ...
	Sr^2 = ... - 2((Ro.x)(So.x) + (Ro.y)(So.y) + (Ro.z)(So.z) + t(Rd.x)(So.x) + t(Rd.y)(So.y) + t(Rd.z)(So.z)) ...
	Sr^2 = ... - 2((Ro.x)(So.x) + (Ro.y)(So.y) + (Ro.z)(So.z) + t((Rd.x)(So.x) + (Rd.y)(So.y) + (Rd.z)(So.z))) ...
	Sr^2 = ... - 2(Ro*So + t(Rd*So)) ...
	Sr^2 = ... + (So.x)^2 + (So.y)^2 + (So.z)^2
	Sr^2 = ... + So*So
	Sr^2 = Ro*Ro + 2t(Ro*Rd) + (t^2)(Rd*Rd) - 2(Ro*So + t(Rd*So)) + So*So
	Sr^2 = Ro*Ro + 2t(Ro*Rd) + (t^2)(Rd*Rd) - 2(Ro*So) - 2t(Rd*So) + So*So
	Sr^2 = (t^2)(Rd*Rd) + 2t(Ro*Rd) - 2t(Rd*So) + Ro*Ro - 2(Ro*So) + So*So
	Sr^2 = (t^2)(Rd*Rd) + 2t(Ro*Rd - Rd*So) + (Ro*Ro - 2(Ro*So) + So*So)
	Sr^2 = (Rd*Rd)t^2 + 2(Ro*Rd - Rd*So)t + (Ro*Ro - 2(Ro*So) + So*So)
	0 = (Rd*Rd)t^2 + 2(Ro*Rd - Rd*So)t + (Ro*Ro - 2(Ro*So) + So*So - Sr^2)
	*/

	// Sphere
	bool HitSphere = false;
	{
		constexpr glm::vec3 SphereOrigin(0.0f, 0.0f, 0.0f);
		constexpr float SphereRadius = 0.5f;

		// These are the a, b, and c components of the quadratic formula.
		// at^2 + bt + c = 0
		const float a = glm::dot(RayDirection, RayDirection);
		const float b
			= 2.0f * (glm::dot(RayOrigin, RayDirection)
				- glm::dot(RayDirection, SphereOrigin));
		const float c
			= glm::dot(RayOrigin, RayOrigin)
				- 2.0f * glm::dot(RayOrigin, SphereOrigin)
				+ glm::dot(SphereOrigin, SphereOrigin)
				- SphereRadius * SphereRadius;

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
