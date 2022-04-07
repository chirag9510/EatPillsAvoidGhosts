#pragma once
#include "AssetManager.h"
#include "GridVertex.h"
#include "FruitType.h"
#include "TileType.h"
#include "AIComponent.h"
#include "ZoneComponent.h"
#include "TextComponent.h"
#include "Maze.h"
#include <entt/entity/registry.hpp>
#include <memory>


class EntityFactory
{
private:
	entt::registry& mRegistry;

public:
	EntityFactory(entt::registry& mRegistry);
	entt::entity PacmanEntity(std::unique_ptr<AssetManager>& pAssetManager,const sf::Vector2f vSpawnPos);
	entt::entity GhostEntity(std::unique_ptr<AssetManager>& pAssetManager, const entt::entity& entityPlayer, const sf::Vector2f vSpawnPos, const GridVertex gridPosCorner, GhostType ghostType, Direction direction);
	entt::entity TileEntity(std::unique_ptr<AssetManager>& pAssetManager, const GridVertex gridPos);
	entt::entity PillEntity(std::unique_ptr<AssetManager>& pAssetManager, const GridVertex gridPos, const TileType tileType, const int iMaxCols);

	entt::entity TeleportTileEntity(Maze& maze, const sf::Vector2f vPosition);
	entt::entity ZoneEntity(sf::IntRect rectZone, ZoneType zoneType);

	entt::entity TextLabelEntity(std::unique_ptr<AssetManager>& pAssetManager, std::string str, const sf::Vector2f vPosition, std::string strFontID, int iFontSize, sf::Color color, TextAlign align = TextAlign::Center_X);

	//bonus items with clyde ai, making player chase them to get them
	entt::entity FruitEntity(std::unique_ptr<AssetManager>& pAssetManager, FruitType fruitType, const sf::Vector2f vSpawnPos);
	entt::entity FruitEntity_ClydeAI(std::unique_ptr<AssetManager>& pAssetManager, FruitType fruitType, const entt::entity& entityPlayer, const sf::Vector2f vSpawnPos, const GridVertex gridPosCorner);

	//score ui when pacman pacman eats ghosts or fruits
	entt::entity BonusPointsEntity(std::unique_ptr <AssetManager>& pAssetManager, const entt::entity entityPlayer);
};

