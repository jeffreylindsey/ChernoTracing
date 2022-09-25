#pragma once

#include <vector>

struct s_Sphere;

/*****************************************************************************/
// s_Scene

struct s_Scene
{
	glm::vec3 BackgroundColor = {0.0f, 0.0f, 0.0f};
	glm::vec3 LightDirection = {0.0f, -1.0f, 0.0f};
	std::vector<s_Sphere> Spheres;
};

/*****************************************************************************/
