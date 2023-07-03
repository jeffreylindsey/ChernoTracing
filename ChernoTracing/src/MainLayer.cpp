#include "MainLayer.h"

#include "Sphere.h"

#include "Walnut/Timer.h"

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

/*****************************************************************************/
// c_MainLayer

/*===========================================================================*/
c_MainLayer::c_MainLayer()
	: m_Image(1, 1, Walnut::ImageFormat::RGBA)
	, m_Renderer(m_Scene, m_Camera)
{
	const size_t BackgroundMaterialIndex = m_Scene.Materials.size();
	m_Scene.Materials.push_back
		( s_Material
			{ .EmissionColor = {0.6f, 0.7f, 0.9f}
			, .EmissionPower = 1.0f
			}
		);

	const size_t GroundMaterialIndex = m_Scene.Materials.size();
	m_Scene.Materials.push_back
		( s_Material{.Color = {0.2f, 0.3f, 1.0f}, .Roughness = 0.1f}
		);

	const size_t CenterSphereMaterialIndex = m_Scene.Materials.size();
	m_Scene.Materials.push_back
		( s_Material{.Color = {1.0f, 0.0f, 1.0f}, .Roughness = 0.0f}
		);

	const size_t RightSphereMaterialIndex = m_Scene.Materials.size();
	m_Scene.Materials.push_back
		( s_Material
			{ .Color = {0.8f, 0.5f, 0.2f}
			, .Roughness = 0.1f
			, .EmissionColor = {0.8f, 0.5f, 0.2f}
			, .EmissionPower = 2.0f
			}
		);

	m_Scene.BackgroundMaterialIndex = BackgroundMaterialIndex;

	m_Scene.Spheres.push_back
		( s_Sphere
			{ .Center = {0.0f, -101.0f, 0.0f}
			, .Radius = 100.0f
			, .MaterialIndex = GroundMaterialIndex
			}
		);
	m_Scene.Spheres.push_back
		( s_Sphere
			{ .Center = {0.0f, 0.0f, 0.0f}
			, .Radius = 1.0f
			, .MaterialIndex = CenterSphereMaterialIndex
			}
		);
	m_Scene.Spheres.push_back
		( s_Sphere
			{ .Center = {2.0f, 0.0f, 0.0f}
			, .Radius = 1.0f
			, .MaterialIndex = RightSphereMaterialIndex
			}
		);

	// Move the camera to default to a different position.
	m_Camera.SetPosition({0.0f, 0.0f, -6.0f});
}

/*=============================================================================
	Overrides Walnut::Layer
-----------------------------------------------------------------------------*/
void c_MainLayer::OnUpdate(float ts)
{
	if (m_Camera.OnUpdate(ts))
		m_Renderer.ResetAccumulation();
}

/*=============================================================================
	Overrides Walnut::Layer
-----------------------------------------------------------------------------*/
void c_MainLayer::OnUIRender()
{
	UIScene();
	UISettings();
	UIViewport();

	Render();
}

/*===========================================================================*/
void c_MainLayer::UIScene()
{
	ImGui::Begin("Scene");

	static constexpr size_t MaterialIndexMin = 0;
	const size_t MaterialIndexMax = m_Scene.Materials.size() - 1;

	for (s_Sphere& r_Sphere : m_Scene.Spheres)
	{
		ImGui::PushID(&r_Sphere);
		ImGui::DragFloat3("Position", glm::value_ptr(r_Sphere.Center), 0.01f);
		ImGui::DragFloat("Radius", &r_Sphere.Radius, 0.01f, 0.0f);
		ImGui::DragScalar
			( "Material"
			, ImGuiDataType_U64
			, &r_Sphere.MaterialIndex
			, 1.0f
			, &MaterialIndexMin
			, &MaterialIndexMax
			);
		ImGui::Separator();
		ImGui::PopID();
	}

	for (s_Material& r_Material : m_Scene.Materials)
	{
		ImGui::PushID(&r_Material);
		ImGui::ColorEdit3("Color", glm::value_ptr(r_Material.Color));
		ImGui::DragFloat("Roughness", &r_Material.Roughness, 0.001f, 0.0f, 1.0f);
		ImGui::ColorEdit3
			( "Emission Color"
			, glm::value_ptr(r_Material.EmissionColor)
			);
		ImGui::DragFloat
			( "Emission Power"
			, &r_Material.EmissionPower
			, 0.05f
			, 0.0f
			, std::numeric_limits<float>::max()
			);
		ImGui::Separator();
		ImGui::PopID();
	}

	ImGui::End();
}

/*===========================================================================*/
void c_MainLayer::UISettings()
{
	ImGui::Begin("Settings");

	ImGui::Text("Last render: %.3fms", m_LastRenderTime);

	if (ImGui::Button("Render"))
		Render();

	ImGui::Checkbox("Accumulate", &m_Renderer.m_UseAccumulation);
	ImGui::Checkbox("Use PCG Hash Random", &m_Renderer.m_UsePCGHashRandom);

	if (ImGui::Button("Reset"))
		m_Renderer.ResetAccumulation();

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

	m_Renderer.Render(m_Image);

	m_LastRenderTime = timer.ElapsedMillis();
}

/*===========================================================================*/
