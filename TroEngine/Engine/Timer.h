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
	float GetStartTimeSec() const;
	void SubstractTimeFromStart(float sec);
	void Stop();
	void PauseOn();
	void PauseOff();
	void SetSpeed(float s);
	float GetSpeed()const;

	bool IsActive();


private:
	mutable Uint32	started_at;
	mutable Uint32	last_read;
	float	speed = 1.f;
	bool	paused = false;
	Uint32  paused_at = 0;
	bool	active = false;
};

#endif //__TIMER_H__