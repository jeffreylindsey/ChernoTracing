#pragma once

#include "Material.h"
#include "Sphere.h"

#include <vector>

/*****************************************************************************/
// s_Scene

struct s_Scene
{
	std::vector<s_Material> Materials;
	size_t BackgroundMaterialIndex = 0;
	std::vector<s_Sphere> Spheres;
};

/*****************************************************************************/
