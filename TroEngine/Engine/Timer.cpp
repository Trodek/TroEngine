// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"
#include "SDL\include\SDL_timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

Timer::~Timer()
{
}

// ---------------------------------------------
void Timer::Start()
{
	started_at = SDL_GetTicks();
	last_read = started_at;
	active = true;
	paused = false;
}

// ---------------------------------------------
Uint32 Timer::Read() const
{
	if (active)
	{
		if (paused)
			return (paused_at - started_at);
		else
		{
			started_at -= (SDL_GetTicks() - last_read)*(speed - 1);
			last_read = SDL_GetTicks();
			return (SDL_GetTicks() - started_at);
		}
			
	}
	else
		return 0;
}

// ---------------------------------------------
float Timer::ReadSec() const
{
	return float(Read()) / 1000.0f;
}

float Timer::GetStartTimeSec() const
{
	return started_at / 1000.0f;
}

void Timer::SubstractTimeFromStart(float sec)
{
	started_at -= (sec * 1000);
}

void Timer::Stop()
{
	active = false;
}

void Timer::PauseOn()
{
	if (paused == false)
	{
		paused_at = SDL_GetTicks();
		paused = true;
	}
}

void Timer::PauseOff()
{
	if (paused == true)
	{
		started_at += (SDL_GetTicks() - paused_at);
		last_read += (SDL_GetTicks() - paused_at);

		paused_at = 0;
		paused = false;
	}
}

void Timer::SetSpeed(float s)
{
	if (active && !paused)
	{
		started_at -= (SDL_GetTicks() - last_read)*(speed - 1);
		last_read = SDL_GetTicks();
	}
	speed = s;
}

float Timer::GetSpeed() const
{
	return speed;
}

bool Timer::IsActive()
{
	return active;
}


