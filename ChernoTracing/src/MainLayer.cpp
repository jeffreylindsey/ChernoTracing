#include "MainLayer.h"

#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "imgui.h"

/*****************************************************************************/
// c_MainLayer

/*=============================================================================
	Overrides Walnut::Layer
-----------------------------------------------------------------------------*/
void c_MainLayer::OnUIRender()
{
	UISettings();
	UIViewport();

	Render();
}

/*===========================================================================*/
void c_MainLayer::UISettings()
{
	ImGui::Begin("Settings");

	ImGui::Text("Last render: %.3fms", m_LastRenderTime);
	if (ImGui::Button("Render"))
		Render();

	ImGui::End();
}

/*===========================================================================*/
void c_MainLayer::UIViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");

	m_ViewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
	m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

	if (m_Image != nullptr)
	{
		ImGui::Image
			( m_Image->GetDescriptorSet()
			,	{ static_cast<float>(m_Image->GetWidth())
				, static_cast<float>(m_Image->GetHeight())
				}
			);
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

/*===========================================================================*/
void c_MainLayer::Render()
{
	Walnut::Timer timer;

	if (m_Image == nullptr
		|| m_ViewportWidth != m_Image->GetWidth()
		|| m_ViewportHeight != m_Image->GetHeight()
	)
	{
		m_Image
			= std::make_unique<Walnut::Image>
				( m_ViewportWidth
				, m_ViewportHeight
				, Walnut::ImageFormat::RGBA
				);

		m_ImageData.resize(m_ViewportWidth * m_ViewportHeight);
	}

	for (s_RGBA& r_Pixel : m_ImageData)
		r_Pixel.Value = Walnut::Random::UInt() | 0xFF000000;

	m_Image->SetData(m_ImageData.data());

	m_LastRenderTime = timer.ElapsedMillis();
}

/*===========================================================================*/
