#pragma once

#include <cstdint>

/*****************************************************************************/
// s_RGBA

struct s_RGBA
{
	union
	{
		uint32_t Value;
		struct
		{
			uint8_t R;
			uint8_t G;
			uint8_t B;
			uint8_t A;
		} Channel;
	};
};
static_assert(sizeof(s_RGBA) == sizeof(uint32_t));

/*****************************************************************************/
