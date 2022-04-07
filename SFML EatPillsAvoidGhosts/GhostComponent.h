#pragma once
#include "GridVertex.h"
#include <entt/entity/fwd.hpp>

enum class Behaviour
{
	Home,
	Leaving,
	Scatter,
	Chase,
	Frightened,
	Eaten											
};

enum class GhostType
{
	Blinky,
	Pinky,
	Inky,
	Clyde
};

struct GhostComponent
{
	bool bHostile;							//hurt player if hostile when in contact. false for fruits which have ghost ai to get away from player
	Behaviour behaviour;
	entt::entity entityPlayer;				//to chase the player
	entt::entity entityBlinky;				//only used by Inky
	GridVertex gridPosScatter;				//the scatter mode grid position of corner 
	GhostComponent(entt::entity entityPlayer, GridVertex gridPosScatter, Behaviour behaviour = Behaviour::Scatter, bool bHostile = true) :
		entityPlayer(entityPlayer),
		gridPosScatter(gridPosScatter),
		behaviour(behaviour), bHostile(bHostile) {}
};
