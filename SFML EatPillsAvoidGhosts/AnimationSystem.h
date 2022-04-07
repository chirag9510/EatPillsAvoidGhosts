#pragma once
#include <entt/entity/registry.hpp>


class AnimationSystem
{
	entt::registry& mRegistry;
	float time;
public:
	AnimationSystem(entt::registry& mRegistry);

	void Update(const float& fDeltaTime);
};

