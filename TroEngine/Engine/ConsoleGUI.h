#ifndef __EDITORCONSOLE__
#define __EDITORCONSOLE__

#include "GUIElement.h"
#include <list>
#include <string>

class ConsoleGUI : public GUIElement
{
public:
	ConsoleGUI();
	~ConsoleGUI();

	update_status UpdateGUI(float dt);
	bool CleanUp();

	void AddLine(const char* new_line);

public:
	bool accept_lines = false;

private:
	void CreateConsole();

private:
	std::list<std::string> console_lines;
	bool scroll_to_bottom = false;

};

#endif // !__EDITORCONSOLE__

