#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructors
	Timer();
	~Timer();

	void Start();
	Uint32 Read() const;
	float ReadSec() const;
	void SubstractTimeFromStart(float sec);
	void Stop();
	void PauseOn();
	void PauseOff();

	bool IsActive();


private:
	Uint32	started_at;
	bool	paused = false;
	Uint32  paused_at = 0;
	bool	active = false;
};

#endif //__TIMER_H__