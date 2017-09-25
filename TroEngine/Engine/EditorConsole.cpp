#include "EditorConsole.h"
#include "imgui.h"

EditorConsole::EditorConsole(): Module(true)
{
}

EditorConsole::~EditorConsole()
{
}

bool EditorConsole::Awake()
{
	accept_lines = true;
	return true;
}

update_status EditorConsole::Update(float dt)
{
	CreateConsole();

	return UPDATE_CONTINUE;
}

bool EditorConsole::CleanUp()
{
	accept_lines = false;
	console_lines.clear();
	return true;
}

void EditorConsole::AddLine(const char * new_line)
{
	std::string line(new_line);

	console_lines.push_back(line);
}

void EditorConsole::CreateConsole() const
{
	ImGui::Begin("Console");

	for (std::list<std::string>::const_iterator line = console_lines.begin(); line != console_lines.end(); ++line)
	{
		ImGui::Text(line->c_str());
	}

	ImGui::End();
}
