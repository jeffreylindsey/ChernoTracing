#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

using namespace Walnut;

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
		Timer timer;

		if (!m_Image || m_ViewportWidth != m_Image->GetWidth() || m_ViewportHeight != m_Image->GetHeight())
		{
			m_Image = std::make_shared<Image>(m_ViewportWidth, m_ViewportHeight, ImageFormat::RGBA);
			m_ImageData.resize(m_ViewportWidth * m_ViewportHeight);
		}

		for (uint32_t& r_Pixel : m_ImageData)
			r_Pixel = Random::UInt() | 0xFF000000;

		m_Image->SetData(m_ImageData.data());

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	std::shared_ptr<Image> m_Image;
	std::vector<uint32_t> m_ImageData;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "ChernoTracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<c_MainLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}