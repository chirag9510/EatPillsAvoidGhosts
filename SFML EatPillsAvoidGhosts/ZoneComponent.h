#pragma once
#include "ZoneType.h"
#include <SFML/Graphics/Rect.hpp>

struct ZoneComponent
{
	sf::IntRect rectZone;
	ZoneType zoneType;
	ZoneComponent(sf::IntRect rectZone, ZoneType zoneType = ZoneType::Slow) : rectZone(rectZone), zoneType(zoneType)
	{}
};