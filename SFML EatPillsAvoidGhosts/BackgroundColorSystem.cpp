#include "BackgroundColorSystem.h"
#include "TileComponent.h"
#include "StaticSpriteComponent.h"

BackgroundColorSystem::BackgroundColorSystem(entt::registry& mRegistry) : mRegistry(mRegistry),
fCurrentTime(0.f), fTimer(10.f), fColorSpeed(5.f)
{
}

void BackgroundColorSystem::Init()
{
	GenerateTargetColor();
	colorCurrent = colorTarget;

	auto group = mRegistry.group<TileComponent, StaticSpriteComponent, ColorTileComponent>();
	for (auto e : group)
		group.get<StaticSpriteComponent>(e).sprite.setColor(colorTarget);
}

void BackgroundColorSystem::Update(const float& fDeltaTime)
{
	//r
	if (colorTarget.r > colorCurrent.r)
	{
		fColor[0] += fColorSpeed * fDeltaTime;
		colorCurrent.r = static_cast<sf::Uint8>(fColor[0]);
		if (colorCurrent.r > colorTarget.r)
			colorCurrent.r = colorTarget.r;
	}
	else if(colorTarget.r < colorCurrent.r)
	{
		fColor[0] -= fColorSpeed * fDeltaTime;
		colorCurrent.r = static_cast<sf::Uint8>(fColor[0]);
		if (colorCurrent.r < colorTarget.r)
			colorCurrent.r = colorTarget.r;
	}
	
	//g
	if (colorTarget.g > colorCurrent.g)
	{
		fColor[1] += fColorSpeed * fDeltaTime;
		colorCurrent.g = static_cast<sf::Uint8>(fColor[1]);
		if (colorCurrent.g > colorTarget.g)
			colorCurrent.g = colorTarget.g;
		
	}
	else if(colorTarget.g < colorCurrent.g)
	{
		fColor[1] -= fColorSpeed * fDeltaTime;
		colorCurrent.g = static_cast<sf::Uint8>(fColor[1]);
		if (colorCurrent.g < colorTarget.g)
			colorCurrent.g = colorTarget.g;
		
	}

	//b
	if (colorTarget.b > colorCurrent.b)
	{
		fColor[2] += fColorSpeed * fDeltaTime;
		colorCurrent.b = static_cast<sf::Uint8>(fColor[2]);
		if (colorCurrent.b > colorTarget.b)
			colorCurrent.b = colorTarget.b;
	}
	else if(colorTarget.b < colorCurrent.b)
	{
		fColor[2] -= fColorSpeed * fDeltaTime;
		colorCurrent.b = static_cast<sf::Uint8>(fColor[2]);
		if (colorCurrent.b < colorTarget.b)
			colorCurrent.b = colorTarget.b;
		
	}
	
	//if both color match then generate new color to chase
	if (colorTarget == colorCurrent)
	{
		/*std::cout << (int)colorTarget.r << "," << (int)colorTarget.g << "," << (int)colorTarget.b << std::endl;
		std::cout << (int)colorCurrent.r << "," << (int)colorCurrent.g << "," << (int)colorCurrent.b << std::endl;
		std::cout << "\n=============\n";*/
		GenerateTargetColor();
	}

	auto group = mRegistry.group<TileComponent, StaticSpriteComponent, ColorTileComponent>();
	for (auto e : group)
		group.get<StaticSpriteComponent>(e).sprite.setColor(colorCurrent);
}

void BackgroundColorSystem::GenerateTargetColor()
{
	std::random_device rd;
	std::mt19937 numGenerator(rd());
	std::uniform_int_distribution<int> distribution(0, 60);

	//rgb focus on 1 color
	std::uniform_int_distribution<int> distribution_rgb(0, 2);

	switch (distribution_rgb(numGenerator))
	{
		//r focus
	case 0: 
		colorTarget.r = 130 + distribution(numGenerator);
		colorTarget.g = 30 + distribution(numGenerator);
		colorTarget.b = 30 + distribution(numGenerator);
		break;
		//g
	case 1:		
		colorTarget.r = 30 + distribution(numGenerator);
		colorTarget.g = 130 + distribution(numGenerator);
		colorTarget.b = 30 + distribution(numGenerator);

		break;
		//b
	case 2:
		colorTarget.r = 30 + distribution(numGenerator);
		colorTarget.g = 30 + distribution(numGenerator);
		colorTarget.b = 130 + distribution(numGenerator);

		break;
	}

	fColor[0] = static_cast<float>(colorCurrent.r);
	fColor[1] = static_cast<float>(colorCurrent.g);
	fColor[2] = static_cast<float>(colorCurrent.b);

}