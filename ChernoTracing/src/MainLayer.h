#pragma once

#include "Renderer.h"

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
		c_MainLayer();

		// Walnut::Layer
		void OnUIRender() override;

	//--- Private Methods -----------------------------------------------------
	private:
		void UISettings();
		void UIViewport();

		void Render();

	//--- Private Members -----------------------------------------------------
	private:
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		Walnut::Image m_Image;

		c_Renderer m_Renderer;

		float m_LastRenderTime = 0.0f;
};

/*****************************************************************************/
