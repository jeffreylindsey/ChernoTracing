#include "MainLayer.h"

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

/*===========================================================================*/
std::unique_ptr<Walnut::Application> Walnut::CreateApplication(int, char**)
{
	const Walnut::ApplicationSpecification Specification
		{ .Name = "ChernoTracing"
		};

	std::unique_ptr<Walnut::Application> Application
		= std::make_unique<Walnut::Application>(Specification);

	Application->PushLayer<c_MainLayer>();

	Application->SetMenubarCallback
		( [&Application]()
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit"))
						Application->Close();

					ImGui::EndMenu();
				}
			}
		);

	return Application;
}

/*===========================================================================*/
