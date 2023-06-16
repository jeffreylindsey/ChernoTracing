#pragma once

#include "Color.h"
#include "Ray.h"

#include <glm/glm.hpp>

#include <optional>
#include <vector>

class c_Camera;
struct s_Scene;
struct s_Sphere;

namespace Walnut
{
	class Image;
}

/*****************************************************************************/
// c_Renderer

class c_Renderer
{
	//--- Public Members ------------------------------------------------------
	public:
		bool m_UseAccumulation = true;

	//--- Public Methods ------------------------------------------------------
	public:
		c_Renderer(const s_Scene& Scene, const c_Camera& Camera);

		void Render(Walnut::Image& r_Image);

		void ResetAccumulation();

	//--- Private Types -------------------------------------------------------
	private:
		struct s_Hit
		{
			const s_Sphere* p_Object = nullptr;
			float Distance = 0.0f;
		};

	//--- Private Static Members ----------------------------------------------
	private:
		static constexpr int MaxBounces = 5;

	//--- Private Methods -----------------------------------------------------
	private:
		glm::vec3 RenderPixel(const glm::vec3& RayDirection) const;
		glm::vec3 RenderRay
			( const s_Ray& Ray
			, const int Bounce
			, const s_Sphere* p_SourceObject
			) const;

		s_Hit FindClosestHit(const s_Ray& Ray, const s_Sphere* p_SourceObject) const;

		glm::vec3 RenderHit
			( const s_Ray& Ray
			, const int Bounce
			, const s_Hit& Hit
			) const;

		s_Hit HitSphere(const s_Ray& Ray, const s_Sphere& Sphere) const;

	//--- Private Members -----------------------------------------------------
	private:
		const s_Scene& m_Scene;
		const c_Camera& m_Camera;

		std::vector<size_t> m_PixelIndexes;

		std::vector<s_RGBA> m_ImageData;

		std::vector<glm::vec3> m_AccumulationData;
		uint64_t m_AccumulationCount = 0;
};

/*****************************************************************************/
