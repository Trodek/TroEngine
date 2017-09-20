#ifndef __EDITORCONSOLE__
#define __EDITORCONSOLE__

#include "Module.h"
#include <list>
#include <string>

class EditorConsole : public Module
{
public:
	EditorConsole();
	~EditorConsole();

	bool Awake();
	update_status Update(float dt);

	void AddLine(const char* new_line);

private:
	void CreateConsole() const;

private:
	std::list<std::string> console_lines;

};

#endif // !__EDITORCONSOLE__

