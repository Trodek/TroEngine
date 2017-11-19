#ifndef __EXPLORER__
#define __EXPLORER__

#include "GUIElement.h"
#include <string>

class Explorer : public GUIElement
{
public:
	Explorer();
	~Explorer();

	update_status UpdateGUI(float dt);

private:
	void CreateExplorer();
	void DrawFolderTree(const char* path);
	void ListFiles();

public:
	bool active = true;

private:
	std::string selected = "";
	std::string selected_file = "";
};

#endif // !__EXPLORER__

