#pragma once

#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"

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
		void OnUpdate(float ts) override;
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

		s_Scene m_Scene;

		c_Camera m_Camera;

		c_Renderer m_Renderer;

		float m_LastRenderTime = 0.0f;
};

/*****************************************************************************/
