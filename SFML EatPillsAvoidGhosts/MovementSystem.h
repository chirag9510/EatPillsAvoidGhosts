#pragma once
#include <entt/entity/registry.hpp>

class MovementSystem
{
public:
	void Update(entt::registry& mRegistry, const float& fDeltaTime);

};

