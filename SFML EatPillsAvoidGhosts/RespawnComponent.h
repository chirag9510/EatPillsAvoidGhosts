#pragma once
#include <SFML/System/Vector2.hpp>

//store original position direction for respawning
struct RespawnComponent
{
	sf::Vector2f vSpawnPos;
	RespawnComponent(sf::Vector2f vSpawnPos = sf::Vector2f(0.f, 0.f)) : vSpawnPos(vSpawnPos) {}
};