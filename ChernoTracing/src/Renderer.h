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
		c_Renderer(const s_Scene& Scene, const c_Camera& Camera);

		void Render(Walnut::Image& r_Image);

	//--- Private Methods -----------------------------------------------------
	private:
		glm::vec3 RenderPixel(const s_Ray& Ray);

		std::optional<float> HitSphere
			( const s_Ray& Ray
			, const s_Sphere& Sphere
			) const;

	//--- Private Members -----------------------------------------------------
	private:
		const s_Scene& m_Scene;
		const c_Camera& m_Camera;

		std::vector<s_RGBA> m_ImageData;
};

/*****************************************************************************/
