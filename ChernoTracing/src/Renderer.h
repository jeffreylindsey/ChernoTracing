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
	//--- Public Methods ------------------------------------------------------
	public:
		void Render
			( const s_Scene& Scene
			, const c_Camera& Camera
			, Walnut::Image& r_Image
			);

	//--- Private Methods -----------------------------------------------------
	private:
		glm::vec3 RenderPixel(const s_Scene& Scene, const s_Ray& Ray);

		std::optional<float> HitSphere
			( const s_Ray& Ray
			, const s_Sphere& Sphere
			) const;

	//--- Private Members -----------------------------------------------------
	private:
		std::vector<s_RGBA> m_ImageData;
};

/*****************************************************************************/
