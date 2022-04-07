#pragma once
#include "Maze.h"
#include "NavigationComponent.h"
#include "AnimationComponent.h"
#include "StaticSpriteComponent.h"
#include <entt/entity/registry.hpp>

class NavigationSystem
{
private:
	entt::registry& mRegistry;
	Maze& maze;

public:
	NavigationSystem(entt::registry& mRegistry, Maze& maze);

	void Update(const float& fDeltaTime);
	void ChangeDirection(NavigationComponent& navComp, AnimationComponent& animComp, StaticSpriteComponent& spriteComp, const sf::Vector2f& vPosition, const GridVertex& gridPos, Direction moveDir);

private:

	void ChangeAnimation(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Direction direction);
	float Distance(sf::Vector2f v1, sf::Vector2f v2) const;

};


