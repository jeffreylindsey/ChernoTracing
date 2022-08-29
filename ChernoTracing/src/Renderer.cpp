#include "Renderer.h"

#include "Camera.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"

/*===========================================================================*/
s_RGBA FloatColorToRGBA(glm::vec4 FloatColor)
{
	FloatColor = glm::clamp(FloatColor, glm::vec4(0.0), glm::vec4(1.0f));

	s_RGBA Result;
	Result.Channel.R = static_cast<uint8_t>(FloatColor.r * 255.0f);
	Result.Channel.G = static_cast<uint8_t>(FloatColor.g * 255.0f);
	Result.Channel.B = static_cast<uint8_t>(FloatColor.b * 255.0f);
	Result.Channel.A = static_cast<uint8_t>(FloatColor.a * 255.0f);

	return Result;
}

/*****************************************************************************/
// c_Renderer

/*===========================================================================*/
void c_Renderer::Render(Walnut::Image& r_Image, const c_Camera& Camera)
{
	const auto& RayDirections = Camera.GetRayDirections();

	// This function assumes there is one ray direction per pixel.
	assert(RayDirections.size() == r_Image.GetWidth() * r_Image.GetHeight());

	m_ImageData.clear();
	m_ImageData.reserve(RayDirections.size());

	for (const glm::vec3& RayDirection : RayDirections)
	{
		m_ImageData.push_back
			( FloatColorToRGBA
				( RenderPixel({Camera.GetPosition(), RayDirection})
				)
			);
	}

	r_Image.SetData(m_ImageData.data());
}

/*===========================================================================*/
glm::vec4 c_Renderer::RenderPixel(const s_Ray& Ray)
{
	const glm::vec3 LightDirection
		= glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f));

	std::optional<glm::vec3> HitColor = RenderSphere(Ray, LightDirection);

	glm::vec4 Result;
	if (HitColor.has_value())
		Result = glm::vec4(HitColor.value(), 1.0f);
	else
		Result = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return Result;
}

/*===========================================================================*/
std::optional<glm::vec3> c_Renderer::RenderSphere
( const s_Ray& Ray
, const glm::vec3& LightDirection
) const
{
	constexpr glm::vec3 SphereOrigin(0.0f, 0.0f, 0.0f);
	constexpr float SphereRadius = 0.5f;
	constexpr glm::vec3 SphereColor(1.0f, 0.0f, 1.0f);

	// Ray: Point = RayOrigin + t*RayDirction, where t >= 0
	// Sphere: magnitude(Point - SphereOrigin) = SphereRadius
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
	0 = (Rd*Rd)t^2 + 2(Rd*(Ro - So))t + (Ro*Ro - 2(Ro*So) + So*So - Sr^2)
	0 = (Rd*Rd)t^2 + 2(Rd*(Ro - So))t + ((Ro - So)*(Ro - So) - Sr^2)
	*/

	const glm::vec3 RaySphereOriginOffset = Ray.Origin - SphereOrigin;

	// These are the a, b, and c components of the quadratic formula.
	// at^2 + bt + c = 0
	const float a = glm::dot(Ray.Direction, Ray.Direction);
	const float b = 2.0f * glm::dot(Ray.Direction, RaySphereOriginOffset);
	const float c
		= glm::dot(RaySphereOriginOffset, RaySphereOriginOffset)
			- SphereRadius * SphereRadius;

	const float Discriminant = b * b - 4.0f * a * c;

	if (Discriminant < 0.0f)
		return std::nullopt;  // No hit.

	// Quadratic formula: t = (-b +- sqrt(Discriminant)) / 2a
	const float t = (-b - glm::sqrt(Discriminant)) / (2.0f * a);

	const glm::vec3 HitPoint = Ray.Origin + t*Ray.Direction;

	const glm::vec3 HitNormal = glm::normalize(HitPoint - SphereOrigin);

	return SphereColor * glm::dot(HitNormal, -LightDirection);
}

/*===========================================================================*/
