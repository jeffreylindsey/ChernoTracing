#pragma once

#include "imgui.h"

#include "Walnut/Image.h"
#include "Walnut/Layer.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

class c_MainLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
		m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

		if (m_Image)
			ImGui::Image(m_Image->GetDescriptorSet(), { static_cast<float>(m_Image->GetWidth()), static_cast<float>(m_Image->GetHeight()) });

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Walnut::Timer timer;

		if (!m_Image || m_ViewportWidth != m_Image->GetWidth() || m_ViewportHeight != m_Image->GetHeight())
		{
			m_Image = std::make_shared<Walnut::Image>(m_ViewportWidth, m_ViewportHeight, Walnut::ImageFormat::RGBA);
			m_ImageData.resize(m_ViewportWidth * m_ViewportHeight);
		}

		for (uint32_t& r_Pixel : m_ImageData)
			r_Pixel = Walnut::Random::UInt() | 0xFF000000;

		m_Image->SetData(m_ImageData.data());

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	std::shared_ptr<Walnut::Image> m_Image;
	std::vector<uint32_t> m_ImageData;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};
