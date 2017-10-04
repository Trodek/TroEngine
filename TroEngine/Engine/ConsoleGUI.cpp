#include "ConsoleGUI.h"
#include "imgui.h"

ConsoleGUI::ConsoleGUI()
{
	accept_lines = true;
}

ConsoleGUI::~ConsoleGUI()
{
}
update_status ConsoleGUI::UpdateGUI(float dt)
{
	CreateConsole();

	return UPDATE_CONTINUE;
}

bool ConsoleGUI::CleanUp()
{
	accept_lines = false;
	console_lines.clear();
	return true;
}

void ConsoleGUI::AddLine(const char * new_line)
{
	std::string line(new_line);

	console_lines.push_back(line);
}

void ConsoleGUI::CreateConsole() const
{
	ImGui::Begin("Console");

	for (std::list<std::string>::const_iterator line = console_lines.begin(); line != console_lines.end(); ++line)
	{
		ImGui::Text(line->c_str());
	}

	ImGui::End();
}
