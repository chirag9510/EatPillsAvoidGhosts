#pragma once
#include "AISystem.h"
#include <entt/entity/registry.hpp>

class AISystem;

struct AIComponent
{
	AIFuncPtr aiFuncPtr;
	AIComponent(AIFuncPtr aiFuncPtr = nullptr) : aiFuncPtr(aiFuncPtr)  {}
};


