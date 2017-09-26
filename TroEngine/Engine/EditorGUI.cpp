#include "EditorGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "imgui.h"

EditorGUI::EditorGUI()
{
}

EditorGUI::~EditorGUI()
{
}

bool EditorGUI::Awake(JSONDoc* config)
{
	return true;
}

update_status EditorGUI::Update(float dt)
{
	CreateGUI();
	AboutPanel();
	GUIConfig();
	MathTest();
	Config();
	
	return UPDATE_CONTINUE;
}

void EditorGUI::CreateGUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Configuration##menu", NULL, &show_config))
			{
			}

			if (ImGui::MenuItem("Close App"))
			{
				App->CloseApp();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Math Test##menu", NULL, &show_math_test))
			{
			}
			if (ImGui::MenuItem("GUI Config", NULL, &show_test_window))
			{

			}
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About", NULL, &show_about))
			{
			}

			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	};
}

void EditorGUI::AboutPanel()
{
	if (show_about)
	{
		ImGui::Begin("TroEngine v0.0.1", &show_about);

		ImGui::Text("3D Engine based on OpenGL 2.1");
		ImGui::Text("Engine mantained by Iban Mas Ortega (Trodek)");
		ImGui::Text("Libraries Used:");
		ImGui::BulletText("SDL2.0");
		ImGui::BulletText("OpenGL 2.1");
		ImGui::BulletText("MathGeoLib");
		ImGui::BulletText("ImGui 1.52");
		ImGui::TextWrapped("This engine is under de MIT License.");

		if (ImGui::Button("GitHub Repository"))
		{
			App->OpenWebPage("https://github.com/Trodek/TroEngine");
		}

		ImGui::End();
	}
}

void EditorGUI::GUIConfig()
{
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void EditorGUI::MathTest()
{
	if (show_math_test)
	{
		ImGui::Begin("Math Test##panel", &show_math_test, ImGuiWindowFlags_AlwaysAutoResize);

		// Rand Float
		if (ImGui::Button("Generate random float"))
		{
			rand_float = App->RandFloat();
		}
		ImGui::SameLine(200);
		ImGui::Text("%.3f", rand_float); 
		ImGui::SameLine(250);
		ImGui::Text("float");

		ImGui::Separator();

		//Rand int range
		ImGui::PushItemWidth(75);
		ImGui::InputInt("Min", &min);
		ImGui::SameLine(125);
		ImGui::PushItemWidth(75);
		ImGui::InputInt("Max", &max);
		if (ImGui::Button("Generate random in range"))
		{
			rand_int = App->RandRange(min, max);
		}
		ImGui::SameLine(200);
		ImGui::Text("%d", rand_int);

		ImGui::Separator();

		//Create x bounding boxes and check contacts
		ImGui::PushItemWidth(75);
		ImGui::InputInt("Min X", &min_x);
		ImGui::SameLine(125);
		ImGui::PushItemWidth(75);
		ImGui::InputInt("Max X", &max_x);

		ImGui::PushItemWidth(75);
		ImGui::InputInt("Min Y", &min_y);
		ImGui::SameLine(125);
		ImGui::PushItemWidth(75);
		ImGui::InputInt("Max Y", &max_y);

		ImGui::PushItemWidth(75);
		ImGui::InputInt("Min Z", &min_z);
		ImGui::SameLine(125);
		ImGui::PushItemWidth(75);
		ImGui::InputInt("Max Z", &max_z);

		ImGui::PushItemWidth(75);
		ImGui::InputInt("Num Objects", &num_obj);
		if (num_obj > 0 && max_x>min_x && max_y>min_y && max_z>min_z)
		{
			if (ImGui::Button("Test objects"))
			{
				//Reset variables
				contacts = 0;
				aabb_objects.clear();
				EDITOR_LOG("--------Math Test---------");

				//Create Objects
				for (int object = 0; object < num_obj; ++object)
				{
					rand_x = App->RandRange(min_x, max_x);
					rand_y = App->RandRange(min_y, max_y);
					rand_z = App->RandRange(min_z, max_z);

					vec min_point(rand_x,rand_z,rand_y);

					rand_x = App->RandRange(min_x, max_x);
					rand_y = App->RandRange(min_y, max_y);
					rand_z = App->RandRange(min_z, max_z);

					vec max_point(rand_x, rand_z, rand_y);

					aabb_objects.push_back(AABB(min_point, max_point));
				}

				EDITOR_LOG("Created %d AABB objects", num_obj);

				//Check Colisions/Contacts
				int curr = 1;
				for (std::list<AABB>::iterator curr_object = aabb_objects.begin(); curr_object != --aabb_objects.end(); ++curr_object)
				{
					int test = curr + 1;
					for (std::list<AABB>::iterator test_object = ++curr_object; test_object != aabb_objects.end(); ++test_object)
					{
						if(curr_object->Intersects(*test_object))
						{
							EDITOR_LOG("Object %d intersects with object %d", curr, test);
							contacts++;
						}
						test++;
					}
					curr++;
				}
				EDITOR_LOG("-------------------------");
			}
			ImGui::SameLine(110);
			ImGui::Text("%d", contacts);
			ImGui::SameLine(125);
			ImGui::Text("Contacts");		
		}

		ImGui::End();
	}
}

void EditorGUI::Config()
{
	if (show_config)
	{
		ImGui::Begin("Configuration##panel", &show_config, ImGuiWindowFlags_AlwaysAutoResize);

		//Ask app for Application config
		App->ConfigGUI();

		//Ask other modules for their config
		App->window->ConfigGUI();
		

		//Ask app for hardware details
		App->HardwareConfig();
		

		ImGui::End();
	}
}
