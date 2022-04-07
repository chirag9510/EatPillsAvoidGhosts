#pragma once
#include <entt/entity/fwd.hpp>

//sprite shown when pacman eats fruits or ghosts
struct BonusPointsComponent
{
	entt::entity entityPlayer;
	BonusPointsComponent(entt::entity entityPlayer) : entityPlayer(entityPlayer)
	{}
};