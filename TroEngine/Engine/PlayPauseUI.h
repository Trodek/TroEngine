#ifndef __PLAYPAUSEUI__
#define __PLAYPAUSEUI__

#include "GUIElement.h"

class PlayPauseUI : public GUIElement
{
public:
	PlayPauseUI();
	~PlayPauseUI();

	update_status UpdateGUI(float dt);

	void CreatePlayPauseUI();
};

#endif // !__PALYPAUSEUI__
