#include "MainLayer.h"

#include "Walnut/Timer.h"

#include "imgui.h"

/*****************************************************************************/
// c_MainLayer

/*===========================================================================*/
c_MainLayer::c_MainLayer()
	: m_Image(1, 1, Walnut::ImageFormat::RGBA)
{
	m_Scene.Spheres.push_back
		( s_Sphere
			{ .Center = {0.0f, 0.0f, 0.0f}
			, .Radius = 0.5f
			, .Color = {1.0f, 0.0f, 1.0f}
			}
		);

	// Move the camera to default to a different position.
	m_Camera.SetPosition({0.0f, 0.0f, -3.0f});
}

/*=============================================================================
	Overrides Walnut::Layer
-----------------------------------------------------------------------------*/
void c_MainLayer::OnUpdate(float ts)
{
	m_Camera.OnUpdate(ts);
}

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

	ImGui::Image
		( m_Image.GetDescriptorSet()
		,	{ static_cast<float>(m_Image.GetWidth())
			, static_cast<float>(m_Image.GetHeight())
			}
		);

	ImGui::End();
	ImGui::PopStyleVar();
}

/*===========================================================================*/
void c_MainLayer::Render()
{
	Walnut::Timer timer;

	m_Image.Resize(m_ViewportWidth, m_ViewportHeight);

	m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);

	m_Renderer.Render(m_Scene, m_Camera, m_Image);

	m_LastRenderTime = timer.ElapsedMillis();
}

/*===========================================================================*/
