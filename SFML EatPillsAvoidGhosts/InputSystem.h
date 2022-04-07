#pragma once
#include <entt/entity/fwd.hpp>
#include <SFML/Window/Event.hpp>
#include <entt/entity/registry.hpp>

class InputSystem
{
public:
	void KeyPressed(const sf::Event& event, entt::registry& mRegistry);
	void KeyReleased(entt::registry& mRegistry);
};

