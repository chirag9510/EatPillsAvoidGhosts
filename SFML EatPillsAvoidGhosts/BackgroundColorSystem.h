//change the sprite color of the background system tiles
#pragma once
#include <entt/entity/registry.hpp>
#include <SFML/Graphics/Color.hpp>
#include <random>

class BackgroundColorSystem
{
	entt::registry& mRegistry;
	sf::Color colorTarget;
	sf::Color colorCurrent;
	float fColor[3];								//color in float useful for precise calculation with deltatime. current color is set using this
	float fColorSpeed;								//speed of changing color

	float fTimer, fCurrentTime;
	

public:
	BackgroundColorSystem(entt::registry& mRegistry);
	void Init();
	void Update(const float& fDeltaTime);

private:
	void GenerateTargetColor();
};

