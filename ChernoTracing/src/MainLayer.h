#pragma once

#include "Walnut/Image.h"
#include "Walnut/Layer.h"

#include <memory>  // std::shared_ptr
#include <vector>

/*****************************************************************************/
// c_MainLayer

class c_MainLayer : public Walnut::Layer
{
	//--- Public Methods ------------------------------------------------------
	public:
		// Walnut::Layer
		void OnUIRender() override;

	//--- Private Types -------------------------------------------------------
	private:
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

	//--- Private Methods -----------------------------------------------------
	private:
		void UISettings();
		void UIViewport();

		void Render();

	//--- Private Members -----------------------------------------------------
	private:
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		std::unique_ptr<Walnut::Image> m_Image;
		std::vector<s_RGBA> m_ImageData;

		float m_LastRenderTime = 0.0f;
};

/*****************************************************************************/
