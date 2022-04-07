#include "RenderSystem.h"
#include "TileComponent.h"
#include "StaticSpriteComponent.h"
#include "TransformComponent.h"
#include "PillComponent.h"
#include "TextComponent.h"
#include "ZoneComponent.h"
#include "FruitComponent.h"
#include "GhostComponent.h"
#include "PacmanComponent.h"
#include "BonusPointsComponent.h"

RenderSystem::RenderSystem()
{
}

void RenderSystem::Update(entt::registry& mRegistry, sf::RenderWindow& mWindow)
{
	mWindow.clear(sf::Color(0, 11, 25));

	//tiles first
	auto groupTiles = mRegistry.group<TileComponent>(entt::get<StaticSpriteComponent>);
	for (auto e : groupTiles)
		mWindow.draw(groupTiles.get<StaticSpriteComponent>(e).sprite);

	auto groupPills = mRegistry.group<PillComponent>(entt::get<StaticSpriteComponent>);
	for (auto e : groupPills)
	{
		if (groupPills.get<StaticSpriteComponent>(e).bDraw)
			mWindow.draw(groupPills.get<StaticSpriteComponent>(e).sprite);
	}

	//then fruits 
	auto groupFruits = mRegistry.group<FruitComponent>(entt::get<StaticSpriteComponent>);
	for (auto e : groupFruits)
	{
		if (groupFruits.get<StaticSpriteComponent>(e).bDraw)
			mWindow.draw(groupFruits.get<StaticSpriteComponent>(e).sprite);
	}

	//pacman
	auto groupPacman = mRegistry.group<PacmanComponent>(entt::get<StaticSpriteComponent>);
	for (auto e : groupPacman)
	{
		if (groupPacman.get<StaticSpriteComponent>(e).bDraw)
			mWindow.draw(groupPacman.get<StaticSpriteComponent>(e).sprite);
	}

	auto groupGhosts = mRegistry.group<GhostComponent>(entt::get<StaticSpriteComponent>);
	for (auto e : groupGhosts)
	{
		if (groupGhosts.get<StaticSpriteComponent>(e).bDraw)
			mWindow.draw(groupGhosts.get<StaticSpriteComponent>(e).sprite);
	}

	auto groupPoints = mRegistry.group<BonusPointsComponent>(entt::get<StaticSpriteComponent>);
	for (auto e : groupPoints)
	{
		if (groupPoints.get<StaticSpriteComponent>(e).bDraw)
			mWindow.draw(groupPoints.get<StaticSpriteComponent>(e).sprite);
	}

	auto viewText = mRegistry.view<TextComponent>();
	for (auto [e, textComp] : viewText.each())
	{
		if (textComp.bDraw)
			mWindow.draw(textComp.text);
	}

	mWindow.display();
}
