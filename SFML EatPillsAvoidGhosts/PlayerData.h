#pragma once
#include <entt/entity/registry.hpp>

class PlayerData
{
public:
	PlayerData();
	unsigned int iScore, iHighScore;
	int iLevel;
	float fTimeSeconds;
};

