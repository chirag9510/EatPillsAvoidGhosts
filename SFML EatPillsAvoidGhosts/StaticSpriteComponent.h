#pragma once
#include <SFML/Graphics/Sprite.hpp>

struct StaticSpriteComponent
{
	sf::Sprite sprite;
	bool bDraw;
	StaticSpriteComponent(sf::Sprite sprite, bool bDraw = true) : sprite(sprite), bDraw(bDraw) {}
};