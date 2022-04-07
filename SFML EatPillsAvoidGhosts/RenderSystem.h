#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <entt/entity/registry.hpp>

class RenderSystem 
{
public:
	RenderSystem();
	void Update(entt::registry& mRegistry, sf::RenderWindow& mWindow);
};

