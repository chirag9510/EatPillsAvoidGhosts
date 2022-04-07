#include "EntityFactory.h"
#include "constants.h"
#include "TileComponent.h"
#include "StaticSpriteComponent.h"
#include "TransformComponent.h"
#include "RespawnComponent.h"
#include "NavigationComponent.h"
#include "InputComponent.h"
#include "PillComponent.h"
#include "CollisionComponent.h"
#include "ChronoComponent.h"
#include "TextComponent.h"
#include "PacmanComponent.h"
#include "constants.h"
#include "FruitComponent.h"
#include "AnimationComponent.h"
#include "UIComponent.h"
#include "BonusPointsComponent.h"

EntityFactory::EntityFactory(entt::registry& mRegistry) : mRegistry(mRegistry)
{}

entt::entity EntityFactory::PacmanEntity(std::unique_ptr<AssetManager>& pAssetManager, const sf::Vector2f vSpawnPos)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<PacmanComponent>(e);
	mRegistry.emplace<TransformComponent>(e, vSpawnPos, GridVertex(static_cast<int>(vSpawnPos.x / TileSize), static_cast<int>(vSpawnPos.y / TileSize)));
	mRegistry.emplace<RespawnComponent>(e, vSpawnPos);
	mRegistry.emplace<InputComponent>(e);
	mRegistry.emplace<CollisionComponent>(e);
	mRegistry.emplace<ChronoComponent>(e);
	mRegistry.emplace<TimerComponent>(e);
	mRegistry.emplace<NavigationComponent>(e, DefaultSpeed);		//actual default speed is set according to the current level set by Maze class so DefaultSpeed value here is irrelevant

	auto& spriteComp = mRegistry.emplace<StaticSpriteComponent>(e, sf::Sprite(pAssetManager->GetTexture("pacman")));
	spriteComp.sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
	spriteComp.sprite.setOrigin(sf::Vector2f(8.f, 8.f));
	spriteComp.sprite.setPosition(vSpawnPos);
	
	auto& animComp = mRegistry.emplace<AnimationComponent>(e);
	animComp.mapAnims["right"].push_back(sf::IntRect(0, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["right"].push_back(sf::IntRect(SpriteSize, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["right"].push_back(sf::IntRect(SpriteSize*2, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["left"].push_back(sf::IntRect(0, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["left"].push_back(sf::IntRect(SpriteSize * 3, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["left"].push_back(sf::IntRect(SpriteSize * 4, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["up"].push_back(sf::IntRect(0, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["up"].push_back(sf::IntRect(SpriteSize * 5, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["up"].push_back(sf::IntRect(SpriteSize * 6, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["down"].push_back(sf::IntRect(0, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["down"].push_back(sf::IntRect(SpriteSize * 7, 0, SpriteSize, SpriteSize));
	animComp.mapAnims["down"].push_back(sf::IntRect(SpriteSize * 8, 0, SpriteSize, SpriteSize));
	for (int i = 0; i < 10; i++)
		animComp.mapAnims["death"].push_back(sf::IntRect(SpriteSize * i, 16, SpriteSize, SpriteSize));
	
	animComp.bReverseAnim = true;
	animComp.strAnim = "left";
	animComp.fAnimTime = .03f;

	return e;
}

entt::entity EntityFactory::GhostEntity(std::unique_ptr<AssetManager>& pAssetManager, const entt::entity& entityPlayer, const sf::Vector2f vSpawnPos, const GridVertex gridPosCorner, GhostType ghostType, Direction direction)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<TransformComponent>(e, vSpawnPos, GridVertex(static_cast<int>(vSpawnPos.x / TileSize), static_cast<int>(vSpawnPos.y / TileSize)));
	mRegistry.emplace<RespawnComponent>(e, vSpawnPos);
	mRegistry.emplace<CollisionComponent>(e);
	mRegistry.emplace<ChronoComponent>(e);
	mRegistry.emplace<TimerComponent>(e);
	mRegistry.emplace<NavigationComponent>(e, DefaultSpeed, direction);		//actual default speed is set according to the current level set by Maze class so DefaultSpeed value here is irrelevant

	int iYSprite = 0;
	mRegistry.emplace<GhostComponent>(e, entityPlayer, gridPosCorner);		//default scatter corners hardcoded
	auto& animComp = mRegistry.emplace<AnimationComponent>(e);
	switch (ghostType)
	{
	case GhostType::Blinky:
		mRegistry.emplace<AIComponent>(e, &AISystem::BlinkyUpdate);
		animComp.fAnimTime = .12f;
		break;
	case GhostType::Pinky:
		mRegistry.emplace<AIComponent>(e, &AISystem::PinkyUpdate);
		iYSprite = 64;
		animComp.fAnimTime = .2f;
		break;
	case GhostType::Inky:
		mRegistry.emplace<AIComponent>(e, &AISystem::InkyUpdate);
		iYSprite = 128;
		animComp.fAnimTime = .17f;
		break;
	case GhostType::Clyde:
		mRegistry.emplace<AIComponent>(e, &AISystem::ClydeUpdate);
		iYSprite = 192;
		animComp.fAnimTime = .25f;
		break;
	}
	
	//anims
	for (int i = 0; i < 10; i++)
		animComp.mapAnims["ghost_right"].push_back(sf::IntRect(SpriteSize * i, iYSprite, SpriteSize, SpriteSize));
	for (int i = 0; i < 10; i++)
		animComp.mapAnims["ghost_left"].push_back(sf::IntRect(SpriteSize * i, iYSprite + 16, SpriteSize, SpriteSize));
	for (int i = 0; i < 10; i++)
		animComp.mapAnims["ghost_up"].push_back(sf::IntRect(SpriteSize * i, iYSprite + 32, SpriteSize, SpriteSize));
	for (int i = 0; i < 10; i++)
		animComp.mapAnims["ghost_down"].push_back(sf::IntRect(SpriteSize * i,iYSprite + 48, SpriteSize, SpriteSize));

	animComp.mapAnims["frightened"].push_back(sf::IntRect(0, 256, SpriteSize, SpriteSize));
	animComp.mapAnims["frightened"].push_back(sf::IntRect(SpriteSize, 256, SpriteSize, SpriteSize));
	animComp.mapAnims["eyes_right"].push_back(sf::IntRect(64, 256, SpriteSize, SpriteSize));
	animComp.mapAnims["eyes_left"].push_back(sf::IntRect(64 + 16, 256, SpriteSize, SpriteSize));
	animComp.mapAnims["eyes_up"].push_back(sf::IntRect(64 + 32, 256, SpriteSize, SpriteSize));
	animComp.mapAnims["eyes_down"].push_back(sf::IntRect(64 + 48, 256, SpriteSize, SpriteSize));
	
	if (direction == Direction(1, 0))
		animComp.strAnim = "ghost_right";
	else if (direction == Direction(-1, 0))
		animComp.strAnim = "ghost_left";
	else if (direction == Direction(0, -1))
		animComp.strAnim = "ghost_up";
	else
		animComp.strAnim = "ghost_down";

	//set first frame
	auto& spriteComp = mRegistry.emplace<StaticSpriteComponent>(e, sf::Sprite(pAssetManager->GetTexture("ghosts")));
	spriteComp.sprite.setOrigin(sf::Vector2f(8.f, 8.f));
	spriteComp.sprite.setPosition(vSpawnPos);
	spriteComp.sprite.setTextureRect(animComp.mapAnims[animComp.strAnim][0]);

	return e;
}

entt::entity EntityFactory::TileEntity(std::unique_ptr<AssetManager>& pAssetManager, const GridVertex gridPos)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<TileComponent>(e, gridPos);
	mRegistry.emplace<StaticSpriteComponent>(e, sf::Sprite(pAssetManager->GetTexture("tileset")));
	return e;
}

entt::entity EntityFactory::PillEntity(std::unique_ptr<AssetManager>& pAssetManager, const GridVertex gridPos, const TileType tileType, const int iMaxCols)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<PillComponent>(e, tileType);
	sf::Vector2f vPosition(gridPos.col * TileSize, gridPos.row * TileSize);
	mRegistry.emplace<TransformComponent>(e, vPosition, gridPos);
	mRegistry.emplace<StaticSpriteComponent>(e, sf::Sprite(pAssetManager->GetTexture("tileset")));
	mRegistry.emplace<CollisionComponent>(e);
	mRegistry.emplace<RespawnComponent>(e, vPosition);
	auto& spriteComp = mRegistry.get<StaticSpriteComponent>(e);
	int i = static_cast<int>(tileType);									//tiletype id matches the sprite index 
	spriteComp.sprite.setTextureRect(sf::IntRect((i % iMaxCols) * TileSize, (i / iMaxCols) * TileSize, TileSize, TileSize));
	spriteComp.sprite.setPosition(sf::Vector2f(gridPos.col * TileSize, gridPos.row * TileSize));
	return e;
}

entt::entity EntityFactory::TeleportTileEntity(Maze& maze, const sf::Vector2f vPosition)
{
	//teleport tiles just mimick normal path tiles and have collision component
	entt::entity e = mRegistry.create();
	GridVertex gridPos(vPosition.x / TileSize, vPosition.y / TileSize);
	mRegistry.emplace<TeleportTileComponent>(e);
	mRegistry.emplace<TransformComponent>(e, vPosition, gridPos);

	//hence they are registered as normal path tile
	maze.setPathVertices.insert(gridPos);
	return e;
}

entt::entity EntityFactory::ZoneEntity(sf::IntRect rectZone, ZoneType zoneType)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<ZoneComponent>(e, rectZone, zoneType);
	return e;
}

entt::entity EntityFactory::TextLabelEntity(std::unique_ptr<AssetManager>& pAssetManager, std::string str, const sf::Vector2f vPosition, std::string strFontID, int iFontSize, sf::Color color, TextAlign align)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<TransformComponent>(e, vPosition);				//just to keep the original string position since the sfml::Text position will be constantly modified while trying to align
	auto& textComp = mRegistry.emplace<TextComponent>(e, align);
	auto& font = pAssetManager->GetFont(strFontID);
	auto& text = textComp.text;
	text.setFont(font);
	text.setString(str);
	text.setFillColor(color);
	//use the default character size
	text.setCharacterSize(8);
	//and then scale it the size wanted. this way font doesnt blur
	text.setScale(sf::Vector2f(static_cast<float>(iFontSize) / 8.f, static_cast<float>(iFontSize) / 8.f));

	int x = vPosition.x, y = vPosition.y;
	//center align
	sf::FloatRect rectText = text.getGlobalBounds();
	if (align == TextAlign::Center)
		x = vPosition.x - rectText.width / 2.f , y = vPosition.y - rectText.height / 2.f;
	else if (align == TextAlign::Center_X)
		x = vPosition.x - rectText.width / 2.f;
	
	text.setPosition(x, y);
	
	///set smooth to false thanks to 
	///https://en.sfml-dev.org/forums/index.php?topic=11912.0
	const_cast<sf::Texture&>(font.getTexture(text.getCharacterSize())).setSmooth(false);

	return e;
}

entt::entity EntityFactory::FruitEntity(std::unique_ptr<AssetManager>& pAssetManager, FruitType fruitType, const sf::Vector2f vSpawnPos)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<TransformComponent>(e, vSpawnPos, GridVertex(static_cast<int>(vSpawnPos.x / TileSize), static_cast<int>(vSpawnPos.y / TileSize)));
	mRegistry.emplace<CollisionComponent>(e);
	auto& spriteComp = mRegistry.emplace<StaticSpriteComponent>(e, sf::Sprite(pAssetManager->GetTexture("fruits")));
	spriteComp.sprite.setOrigin(sf::Vector2f(8.f, 8.f));
	spriteComp.sprite.setPosition(vSpawnPos);
	switch (fruitType)
	{
	case FruitType::Cherry:
		spriteComp.sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 100);
		break;
	case FruitType::Strawberry:
		spriteComp.sprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 300);
		break;
	case FruitType::Orange:
		spriteComp.sprite.setTextureRect(sf::IntRect(32, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 500);
		break;
	case FruitType::Apple:
		spriteComp.sprite.setTextureRect(sf::IntRect(48, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 700);
		break;
	case FruitType::Melon:
		spriteComp.sprite.setTextureRect(sf::IntRect(64, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 1000);
		break;
	case FruitType::GalaxianStarship:
		spriteComp.sprite.setTextureRect(sf::IntRect(80, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 2000);
		break;
	case FruitType::Bell:
		spriteComp.sprite.setTextureRect(sf::IntRect(96, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 3000);
		break;
	case FruitType::Key:
		spriteComp.sprite.setTextureRect(sf::IntRect(112, 0, 16, 16));
		mRegistry.emplace<FruitComponent>(e, 5000);
		break;
	}

	return e;
}

entt::entity EntityFactory::FruitEntity_ClydeAI(std::unique_ptr<AssetManager>& pAssetManager, FruitType fruitType, const entt::entity& entityPlayer, const sf::Vector2f vSpawnPos, const GridVertex gridPosCorner)
{
	entt::entity e = FruitEntity(pAssetManager, fruitType, vSpawnPos);
	mRegistry.emplace<GhostComponent>(e, entityPlayer, gridPosCorner, Behaviour::Scatter, false);
	mRegistry.emplace<NavigationComponent>(e, DefaultSpeed, Direction(1, 0));
	mRegistry.emplace<AIComponent>(e, &AISystem::ClydeUpdate);
	return e;
}


entt::entity EntityFactory::BonusPointsEntity(std::unique_ptr<AssetManager>& pAssetManager, const entt::entity entityPlayer)
{
	entt::entity e = mRegistry.create();
	mRegistry.emplace<BonusPointsComponent>(e, entityPlayer);
	mRegistry.emplace<ChronoComponent>(e);
	mRegistry.emplace<TimerComponent>(e);
	auto& spriteComp = mRegistry.emplace<StaticSpriteComponent>(e, sf::Sprite(pAssetManager->GetTexture("score")), false);
	spriteComp.sprite.setOrigin(sf::Vector2f(10.f, 8.f));

	//anim comp is used to display points sprite as single frame animations. 
	auto& animComp = mRegistry.emplace<AnimationComponent>(e, 4.f);
	animComp.strAnim = "200";
	animComp.mapAnims["200"].push_back(sf::IntRect(0, 0, 20, 16));
	animComp.mapAnims["400"].push_back(sf::IntRect(20, 0, 20, 16));
	animComp.mapAnims["800"].push_back(sf::IntRect(40, 0, 20, 16));
	animComp.mapAnims["1600"].push_back(sf::IntRect(60, 0, 20, 16));
	
	animComp.mapAnims["100"].push_back(sf::IntRect(80, 0, 20, 16));
	animComp.mapAnims["300"].push_back(sf::IntRect(100, 0, 20, 16));
	animComp.mapAnims["500"].push_back(sf::IntRect(120, 0, 20, 16));
	animComp.mapAnims["700"].push_back(sf::IntRect(140, 0, 20, 16));
	animComp.mapAnims["1000"].push_back(sf::IntRect(160, 0, 20, 16));
	animComp.mapAnims["2000"].push_back(sf::IntRect(180, 0, 20, 16));
	animComp.mapAnims["3000"].push_back(sf::IntRect(200, 0, 20, 16));
	animComp.mapAnims["5000"].push_back(sf::IntRect(220, 0, 20, 16));

	return e;
}


