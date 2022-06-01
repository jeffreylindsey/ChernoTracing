#include "MainLayer.h"

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

/*===========================================================================*/
std::unique_ptr<Walnut::Application> Walnut::CreateApplication(int, char**)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "ChernoTracing";

	std::unique_ptr<Walnut::Application> app = std::make_unique<Walnut::Application>(spec);
	app->PushLayer<c_MainLayer>();
	app->SetMenubarCallback([&app]()
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

/*===========================================================================*/
