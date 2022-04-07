#pragma once
#include "ChronoSystem.h"
#include <entt/entity/registry.hpp>

struct ChronoComponent
{
	//funx pointer for chronosystem funx that are executed in the update method
	ChronoFncPtr chronoFncPtr;
	ChronoComponent()
	{
		chronoFncPtr = nullptr;
	}
};

//paired with ChronoComponent
struct TimerComponent
{
	//fTimerLifeTime is the lifetime of the timer funx set for entity after which it will remove itself or be repeated
	float fTimerLifeTime, fTimerSecondary;									//secondary timer can be used by chronofunx to time another timer simultanously with the normal one
	float fCurrentTime;
	TimerComponent() : fCurrentTime(0.f), fTimerLifeTime(0.f), fTimerSecondary(0.f) {}
};

