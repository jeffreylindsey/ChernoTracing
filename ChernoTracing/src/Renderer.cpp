#include "Renderer.h"

#include "Camera.h"
#include "Scene.h"
#include "Sphere.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"

#include <algorithm>
#include <execution>
#include <ranges>

/*===========================================================================*/
s_RGBA FloatColorToRGBA(glm::vec3 FloatColor)
{
	FloatColor = glm::clamp(FloatColor, glm::vec3(0.0), glm::vec3(1.0f));

	s_RGBA Result;
	Result.Channel.R = static_cast<uint8_t>(FloatColor.r * 255.0f);
	Result.Channel.G = static_cast<uint8_t>(FloatColor.g * 255.0f);
	Result.Channel.B = static_cast<uint8_t>(FloatColor.b * 255.0f);
	Result.Channel.A = 255;

	return Result;
}

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

/*===========================================================================*/
uint32_t PCGHash(const uint32_t Input)
{
	const uint32_t State = Input * 747796405u + 289133453u;
	const uint32_t Word = ((State >> ((State >> 28u) + 4u)) ^ State) * 277803737u;
	return (Word >> 22u) ^ Word;
}

/*===========================================================================*/
float PCGHashFloat(uint32_t& r_State)
{
	r_State = PCGHash(r_State);

	return static_cast<float>(r_State)
		/ static_cast<float>(std::numeric_limits<uint32_t>::max());
}

/*===========================================================================*/
glm::vec3 PCGHashUnitSphere(uint32_t& r_State)
{
	return glm::normalize
		( glm::vec3
			( 2.0f * PCGHashFloat(r_State) - 1.0f
			, 2.0f * PCGHashFloat(r_State) - 1.0f
			, 2.0f * PCGHashFloat(r_State) - 1.0f
			)
		);
}

/*****************************************************************************/
// c_Renderer

/*===========================================================================*/
c_Renderer::c_Renderer(const s_Scene& Scene, const c_Camera& Camera)
	: m_Scene(Scene)
	, m_Camera(Camera)
{
}

/*===========================================================================*/
void c_Renderer::Render(Walnut::Image& r_Image)
{
	const auto& RayDirections = m_Camera.GetRayDirections();

	const size_t NumPixels = RayDirections.size();

	// This function assumes there is one ray direction per pixel.
	assert(NumPixels == r_Image.GetWidth() * r_Image.GetHeight());

	if (m_PixelIndexes.size() != NumPixels)
	{
		m_PixelIndexes.resize(NumPixels);
		for (size_t PixelIndex = 0; PixelIndex < NumPixels; ++PixelIndex)
			m_PixelIndexes[PixelIndex] = PixelIndex;
	}

	m_ImageData.clear();
	m_ImageData.resize(NumPixels);

	if (!m_UseAccumulation || m_AccumulationData.size() != NumPixels)
		ResetAccumulation();

	if (m_AccumulationCount == 0)
	{
		// Note: Clearing and resizing will all of the accumulation data to 0.
		m_AccumulationData.clear();
		m_AccumulationData.resize(NumPixels);
	}

	++m_AccumulationCount;

	std::for_each
		( std::execution::par_unseq
		, m_PixelIndexes.begin()
		, m_PixelIndexes.end()
		, [&](const size_t PixelIndex)
			{
				const glm::vec3& RayDirection = RayDirections[PixelIndex];

				uint32_t PCGHashState
					= static_cast<uint32_t>(PixelIndex * m_AccumulationCount);

				glm::vec3& r_AccumulationPixel
					= m_AccumulationData[PixelIndex];

				r_AccumulationPixel += RenderPixel(RayDirection, PCGHashState);

				const glm::vec3 AccumulationAverageColor
					= r_AccumulationPixel / static_cast<float>(m_AccumulationCount);

				m_ImageData[PixelIndex] = FloatColorToRGBA(AccumulationAverageColor);
			}
		);

	r_Image.SetData(m_ImageData.data());
}

/*=============================================================================
	ResetFrameIndex
-----------------------------------------------------------------------------*/
void c_Renderer::ResetAccumulation()
{
	m_AccumulationCount = 0;
}

/*=============================================================================
	PerPixel
-----------------------------------------------------------------------------*/
glm::vec3 c_Renderer::RenderPixel
( const glm::vec3& RayDirection
, uint32_t& r_PCGHashState
) const
{
	return RenderRay
		( s_Ray{m_Camera.GetPosition(), RayDirection}
		, 0
		, nullptr
		, r_PCGHashState
		);
}

/*===========================================================================*/
glm::vec3 c_Renderer::RenderRay
( const s_Ray& Ray
, const int Bounce
, const s_Sphere* p_SourceObject
, uint32_t& r_PCGHashState
) const
{
	const s_Hit ClosestHit = FindClosestHit(Ray, p_SourceObject);

	if (ClosestHit.p_Object == nullptr)
	{
		const s_Material& BackgroundMaterial
			= m_Scene.Materials[m_Scene.BackgroundMaterialIndex];

		return BackgroundMaterial.EmissionColor
			* BackgroundMaterial.EmissionPower;
	}

	return RenderHit(Ray, Bounce, ClosestHit, r_PCGHashState);
}

/*=============================================================================
	TraceRay
-----------------------------------------------------------------------------*/
c_Renderer::s_Hit c_Renderer::FindClosestHit
( const s_Ray& Ray
, const s_Sphere* p_SourceObject
) const
{
	s_Hit ClosestHit = {nullptr, std::numeric_limits<float>::max()};

	for (const s_Sphere& Sphere : m_Scene.Spheres)
	{
		// This is to avoid possibly hitting the object that the ray already
		// bounced off of.
		if (&Sphere == p_SourceObject)
			continue;

		const s_Hit Hit = HitSphere(Ray, Sphere);
		if (Hit.p_Object == nullptr)
			continue;

		if (ClosestHit.Distance > Hit.Distance)
			ClosestHit = Hit;
	}

	return ClosestHit;
}

/*=============================================================================
	ClosestHit
-----------------------------------------------------------------------------*/
glm::vec3 c_Renderer::RenderHit
( const s_Ray& Ray
, const int Bounce
, const s_Hit& Hit
, uint32_t& r_PCGHashState
) const
{
	const s_Material& HitMaterial
		= m_Scene.Materials[Hit.p_Object->MaterialIndex];

	// The light that this object emits.
	const glm::vec3 EmissionLight
		= HitMaterial.EmissionColor * HitMaterial.EmissionPower;

	// Light from elsewhere that gets bounced off of this object.
	glm::vec3 BounceLight = {};
	if (Bounce < MaxBounces)
	{
		const glm::vec3 HitPoint = Ray.Origin + Hit.Distance * Ray.Direction;

		const glm::vec3 HitNormal
			= glm::normalize(HitPoint - Hit.p_Object->Center);

		const glm::vec3 RandomDirection
			= m_UsePCGHashRandom
				? PCGHashUnitSphere(r_PCGHashState)
				: Walnut::Random::InUnitSphere();

		const glm::vec3 BounceDirection
			= glm::normalize(HitNormal + RandomDirection);

		const s_Ray BounceRay{HitPoint, BounceDirection};

		BounceLight
			= RenderRay(BounceRay, Bounce + 1, Hit.p_Object, r_PCGHashState);

		// Filter the bounce light through this object's color.
		BounceLight *= HitMaterial.Color;
	}

	return EmissionLight + BounceLight;
}

/*===========================================================================*/
c_Renderer::s_Hit c_Renderer::HitSphere
( const s_Ray& Ray
, const s_Sphere& Sphere
) const
{
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

	const glm::vec3 RaySphereCenterOffset = Ray.Origin - Sphere.Center;

	// These are the a, b, and c components of the quadratic formula.
	// at^2 + bt + c = 0
	const float a = glm::dot(Ray.Direction, Ray.Direction);
	const float b = 2.0f * glm::dot(Ray.Direction, RaySphereCenterOffset);
	const float c
		= glm::dot(RaySphereCenterOffset, RaySphereCenterOffset)
			- Sphere.Radius * Sphere.Radius;

	const float Discriminant = b * b - 4.0f * a * c;

	if (Discriminant < 0.0f)
		return {nullptr};  // No hit.

	// Quadratic formula: t = (-b +- sqrt(Discriminant)) / 2a
	const float t = (-b - glm::sqrt(Discriminant)) / (2.0f * a);

	// Do not hit if the near side is behind the ray origin.
	if (t < 0.0f)
		return {nullptr};

	return {&Sphere, t};
}

/*===========================================================================*/
