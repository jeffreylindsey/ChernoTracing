#pragma once

#include "Walnut/Image.h"
#include "Walnut/Layer.h"

#include <memory>  // std::shared_ptr
#include <vector>

class c_MainLayer : public Walnut::Layer
{
public:
	void OnUIRender() override;

	void Render();

private:
	std::shared_ptr<Walnut::Image> m_Image;
	std::vector<uint32_t> m_ImageData;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};
