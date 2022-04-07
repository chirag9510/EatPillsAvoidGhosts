//a system for all components related to navigation, tiles etc.
#pragma once
#include "Maze.h"
#include "Direction.h"
#include "AnimationComponent.h"
#include "StaticSpriteComponent.h"
#include <vector>
#include <entt/entity/registry.hpp>

struct Direction;

class AISystem;

//for components
typedef GridVertex(AISystem::* AIFuncPtr)(const entt::entity& entity);

class AISystem
{
private:
	GridVertex gridPosHomeCenter;								//when a frightened ghost is eaten, it targets this grid position which is the center of the home (maze)					
	GridVertex gridPosGate;										//the gate which is the target position when behaviour goes from home to leaving
	int iYExitRow;												//the row of tiles that are at the Gate of the pen where ghosts live. to dictate behaviour change from Leaving to Chase
	Maze& maze;
	entt::registry& mRegistry;
	std::vector<Direction> vecAllDirections;

public:
	AISystem(entt::registry& mRegistry, Maze& maze);
	~AISystem() = default;
	
	//the way ai navigates through the maze is same only difference is their target tiles which are recieved from
	//AIFuncPtr stored in their AIComponents
	void Update(const float& fDeltaTime);
	
	GridVertex BlinkyUpdate(const entt::entity& entity);		//chase directly at target
	GridVertex PinkyUpdate(const entt::entity& entity);
	GridVertex InkyUpdate(const entt::entity& entity);
	GridVertex ClydeUpdate(const entt::entity& entity);

private:

	void ChangeAnimationWithDirection(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Behaviour behaviour, Direction direction);
	int Distance(const GridVertex& gridPos, const GridVertex& gridPosTarget) const;
	float DistanceVector2f(const sf::Vector2f& v1, const sf::Vector2f& v2) const;
};

