#pragma once
#include "GridVertex.h"
#include <SFML/System/Vector2.hpp>

struct TransformComponent
{
	sf::Vector2f vPosition;
	GridVertex gridPos;
	TransformComponent(sf::Vector2f vPosition = sf::Vector2f(0.f, 0.f), GridVertex gridPos = GridVertex(0, 0)) : vPosition(vPosition), gridPos(gridPos)
	{}
};
