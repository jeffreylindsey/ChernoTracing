#pragma once

#include <glm/glm.hpp>

/*****************************************************************************/
// s_Material

struct s_Material
{
	glm::vec3 Color = {};
	float Roughness = 0.0f;
	//float Metallic = 0.0f;
	glm::vec3 EmissionColor = {};
	float EmissionPower = 0.0f;
};

/*****************************************************************************/
