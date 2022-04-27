#include "Level.h"
#include "EntityFactory.h"
#include "constants.h"
#include "PacmanComponent.h"
#include "CheckpointComponent.h"
#include "CollisionComponent.h"
#include "PillComponent.h"
#include "StaticSpriteComponent.h"
#include "TransformComponent.h"
#include <ctime>
#include <random>

Level::Level() : iCheckpoints(0)
{}
FruitType Level::GetRandomizedFruitType()
{
	std::random_device rd;
	std::mt19937 numGenerator(rd());
	std::uniform_int_distribution<int> distribution(0, static_cast<int>(FruitType::Total) - 1);
	return static_cast<FruitType>(distribution(numGenerator));
}
bool Level::HasCheckpoints()
{
	return iCheckpoints;
}
void Level::Destroy(entt::registry& mRegistry)
{
	//destroy all entities loaded by this level like pills and ghosts
	for (auto e : vecEntities)
		mRegistry.destroy(e);
}

//***Classic
Classic::Classic(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Pills Avoid Ghosts";
}
void Classic::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{


	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles
	//get player entity	
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills and power pills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::Pill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::PowerPill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

	//ghosts
	auto entityBlinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Blinky, Direction(1, 0));
	vecEntities.push_back(entityBlinky);	
	
	auto entityInky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Inky, Direction(-1, 0));
	mRegistry.patch<GhostComponent>(entityInky, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky);

	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[2], maze->vecScatterCorners[2], GhostType::Pinky, Direction(1, 0)));
	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[3], maze->vecScatterCorners[3], GhostType::Clyde, Direction(-1, 0)));
}

void Classic::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Classic::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Classic::GetObjectiveString() const { return strObjective; }



//****************************
//***Classic No powerPills
Classic_NoPowerPills::Classic_NoPowerPills(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Pills Avoid Ghosts";
}
void Classic_NoPowerPills::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles
	//get player entity	
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills even at the location of powerpills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::Pill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::Pill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

	//ghosts
	auto entityBlinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Blinky, Direction(1, 0));
	vecEntities.push_back(entityBlinky);

	auto entityInky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Inky, Direction(-1, 0));
	mRegistry.patch<GhostComponent>(entityInky, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky);

	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[2], maze->vecScatterCorners[2], GhostType::Pinky, Direction(1, 0)));
	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[3], maze->vecScatterCorners[3], GhostType::Clyde, Direction(-1, 0)));
}
void Classic_NoPowerPills::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Classic_NoPowerPills::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Classic_NoPowerPills::GetObjectiveString() const { return strObjective; }


//****************************
//***Classic PowerPill Only
Classic_PowerPillOnly::Classic_PowerPillOnly(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Pills Avoid Ghosts";
}
void Classic_PowerPillOnly::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles
	//get player entity	
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills and power pills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::PowerPill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::PowerPill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

	//ghosts
	auto entityBlinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Blinky, Direction(1, 0));
	vecEntities.push_back(entityBlinky);

	auto entityInky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Inky, Direction(-1, 0));
	mRegistry.patch<GhostComponent>(entityInky, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky);

	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[2], maze->vecScatterCorners[2], GhostType::Pinky, Direction(1, 0)));
	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[3], maze->vecScatterCorners[3], GhostType::Clyde, Direction(-1, 0)));
}
void Classic_PowerPillOnly::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Classic_PowerPillOnly::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Classic_PowerPillOnly::GetObjectiveString() const { return strObjective; }



//****************************
//************Classic 1 blinky 2 Inkies
Classic_BlinkyInky::Classic_BlinkyInky(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Pills Avoid Ghosts";
}
void Classic_BlinkyInky::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles
	//get player entity	
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills and power pills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::Pill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::PowerPill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

	//ghosts
	auto entityBlinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Blinky, Direction(1, 0));
	vecEntities.push_back(entityBlinky);
	
	//2 inkies
	auto entityInky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Inky, Direction(-1, 0));
	mRegistry.patch<GhostComponent>(entityInky, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky);

	auto entityInky2 = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[2], maze->vecScatterCorners[2], GhostType::Inky, Direction(1,0));
	mRegistry.patch<GhostComponent>(entityInky2, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky2);
}
void Classic_BlinkyInky::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Classic_BlinkyInky::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Classic_BlinkyInky::GetObjectiveString() const { return strObjective; }


//***********************************
//************Classic 4 Clydes
Classic_Clydes::Classic_Clydes(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Pills Avoid Ghosts";

}
void Classic_Clydes::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles
	//get player entity	
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills and power pills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::Pill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::PowerPill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

	//Clydes
	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Clyde, Direction(1, 0)));

	//3 other clydes
	Direction direction(-1, 0);
	for (int i = 1; i < 4; i++)
	{
		vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[i], maze->vecScatterCorners[i], GhostType::Clyde, direction));
		direction *= -1;
	}
}
void Classic_Clydes::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Classic_Clydes::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Classic_Clydes::GetObjectiveString() const { return strObjective; }


//***********************************
//************Classic 4 Pinkies
Classic_Pinkies::Classic_Pinkies(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Pills Avoid Ghosts";
}
void Classic_Pinkies::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles
	//get player entity	
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills and power pills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::Pill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::PowerPill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

	//3 other Pinkies
	Direction direction(1, 0);
	for (int i = 0; i < 3; i++)
	{
		vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[i], maze->vecScatterCorners[i], GhostType::Pinky, direction));
		direction *= -1;
	}
}
void Classic_Pinkies::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Classic_Pinkies::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Classic_Pinkies::GetObjectiveString() const { return strObjective; }


//*******************
//******TreasureRoom
TreasureRoom::TreasureRoom(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Fruits";
}
void TreasureRoom::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);

	//load pills and power pills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.FruitEntity(pAssetManager, GetRandomizedFruitType(), sf::Vector2f(pillPos.col * TileSize + TileSize_Half, pillPos.row * TileSize + TileSize_Half));
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.FruitEntity(pAssetManager, GetRandomizedFruitType(), sf::Vector2f(pillPos.col * TileSize + TileSize_Half, pillPos.row * TileSize + TileSize_Half));
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
}
void TreasureRoom::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void TreasureRoom::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string TreasureRoom::GetObjectiveString() const { return strObjective; }


//*******************
//******TreasureRoom Clydes
TreasureRoom_ClydesChase::TreasureRoom_ClydesChase(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Fruits Avoid Ghosts";
}
void TreasureRoom_ClydesChase::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	//get player entity	
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills and power pills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.FruitEntity(pAssetManager, GetRandomizedFruitType(), sf::Vector2f(pillPos.col * TileSize + TileSize_Half, pillPos.row * TileSize + TileSize_Half));
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}
	for (auto pillPos : maze->vecPowerPillVertices)
	{
		auto e = factory.FruitEntity(pAssetManager, GetRandomizedFruitType(), sf::Vector2f(pillPos.col * TileSize + TileSize_Half, pillPos.row * TileSize + TileSize_Half));
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

//	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Clyde));

	//3 clydes
	Direction direction(1, 0);
	for (int i = 0; i < 3; i++)
	{
		vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[i], maze->vecScatterCorners[i], GhostType::Clyde, direction));
		direction *= -1;
	}
}
void TreasureRoom_ClydesChase::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void TreasureRoom_ClydesChase::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string TreasureRoom_ClydesChase::GetObjectiveString() const { return strObjective; }



//*******************
//******Treasure Clyde AI
Treasure_ClydeAI::Treasure_ClydeAI(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Fruits";
}
void Treasure_ClydeAI::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	//get player entity
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	for (int i = 0; i < maze->vecGenericSpawnPoints.size(); i++)
	{
		auto e = factory.FruitEntity_ClydeAI(pAssetManager, GetRandomizedFruitType(), entityPlayer, maze->vecGenericSpawnPoints[i], maze->vecScatterCorners[i / 2]);
		vecEntities.push_back(e);
		mRegistry.emplace<CheckpointComponent>(e);
		iCheckpoints++;
	}
}
void Treasure_ClydeAI::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Treasure_ClydeAI::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Treasure_ClydeAI::GetObjectiveString() const { return strObjective; }


//*******************
//******Treasure Clyde AI and 3 clydes chasing
Treasure_ClydesChase::Treasure_ClydesChase(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Fruits Avoid Ghosts";

}
void Treasure_ClydesChase::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	//get player entity
	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	for (int i = 0; i < maze->vecGenericSpawnPoints.size(); i++)
	{
		auto e = factory.FruitEntity_ClydeAI(pAssetManager, GetRandomizedFruitType(), entityPlayer, maze->vecGenericSpawnPoints[i], maze->vecScatterCorners[i / 2]);
		vecEntities.push_back(e);
		mRegistry.emplace<CheckpointComponent>(e);
		iCheckpoints++;
	}

	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Clyde, Direction(1, 0)));
	vecEntities.push_back(factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Clyde, Direction(-1, 0)));

}
void Treasure_ClydesChase::Update(entt::registry& mRegistry, const float& fDeltaTime) {}
void Treasure_ClydesChase::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string Treasure_ClydesChase::GetObjectiveString() const { return strObjective; }


//****************************
//********Eat all Ghosts

//eat all 4 ghosts and 8 fruits
EatGhosts::EatGhosts(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Pills and Ghosts";
}
void EatGhosts::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//set timer to respawn powerpill at the center
	fTimer = 15.f;
	fCurrentTime = 0.f;

	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles

	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//load pills even at the location of powerpills
	for (auto pillPos : maze->vecPillVertices)
	{
		auto e = factory.PillEntity(pAssetManager, pillPos, TileType::Pill, iMaxCols);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
	}

	//spawn ghosts with checkpoints
	auto entityBlinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Blinky, Direction(1, 0));
	vecEntities.push_back(entityBlinky);
	mRegistry.emplace<CheckpointComponent>(entityBlinky);
	iCheckpoints++;

	auto entityInky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Inky, Direction(-1, 0));
	mRegistry.patch<GhostComponent>(entityInky, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky);
	mRegistry.emplace<CheckpointComponent>(entityInky);
	iCheckpoints++;

	auto entityPinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[2], maze->vecScatterCorners[2], GhostType::Pinky, Direction(1, 0));
	vecEntities.push_back(entityPinky);
	mRegistry.emplace<CheckpointComponent>(entityPinky);
	iCheckpoints++;

	auto entityClyde = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[3], maze->vecScatterCorners[3], GhostType::Clyde, Direction(-1, 0));
	vecEntities.push_back(entityClyde);
	mRegistry.emplace<CheckpointComponent>(entityClyde);
	iCheckpoints++;

	//power pill at the center
	std::random_device rd;
	std::mt19937 numGenerator(rd());
	std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
	entityPowerPill = factory.PillEntity(pAssetManager, maze->vecPowerPillVertices[distribution(numGenerator)], TileType::PowerPill, iMaxCols);
	vecEntities.push_back(entityPowerPill);
}
void EatGhosts::Update(entt::registry& mRegistry, const float& fDeltaTime)
{
	fCurrentTime += fDeltaTime;
	if (fCurrentTime >= fTimer)
	{
		fCurrentTime -= fTimer;

		//respawn it with updated components if it isnt already present in the level
		if (!mRegistry.all_of<PillComponent>(entityPowerPill))
		{
			mRegistry.emplace<CollisionComponent>(entityPowerPill);
			mRegistry.emplace<PillComponent>(entityPowerPill, TileType::PowerPill);

			//randomize position for transformComponent of powerpill
			std::random_device rd;
			std::mt19937 numGenerator(rd());
			std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
			int index = distribution(numGenerator);
			auto gridPos = maze->vecPowerPillVertices[index];
			sf::Vector2f vPosition(gridPos.col * TileSize, gridPos.row * TileSize);

			mRegistry.patch<TransformComponent>(entityPowerPill, [gridPos, vPosition](auto& transformComp)
				{
					transformComp.vPosition = vPosition;
					transformComp.gridPos = gridPos;
				});

			mRegistry.patch<StaticSpriteComponent>(entityPowerPill, [vPosition](auto& spriteComp)
				{
					spriteComp.sprite.setPosition(vPosition);
					spriteComp.bDraw = true;
				});
		}
	}
}
void EatGhosts::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string EatGhosts::GetObjectiveString() const { return strObjective; }


//eat all 4 ghosts (no pills)
EatGhosts_NoPills::EatGhosts_NoPills(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Ghosts";
}
void EatGhosts_NoPills::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//set timer to respawn powerpill at the center
	fTimer = 15.f;
	fCurrentTime = 0.f;

	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles

	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	//spawn ghosts with checkpoints
	auto entityBlinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Blinky, Direction(1, 0));
	vecEntities.push_back(entityBlinky);
	mRegistry.emplace<CheckpointComponent>(entityBlinky);
	iCheckpoints++;

	auto entityInky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Inky, Direction(-1, 0));
	mRegistry.patch<GhostComponent>(entityInky, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky);
	mRegistry.emplace<CheckpointComponent>(entityInky);
	iCheckpoints++;

	auto entityPinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[2], maze->vecScatterCorners[2], GhostType::Pinky, Direction(1, 0));
	vecEntities.push_back(entityPinky);
	mRegistry.emplace<CheckpointComponent>(entityPinky);
	iCheckpoints++;

	auto entityClyde = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[3], maze->vecScatterCorners[3], GhostType::Clyde, Direction(-1, 0));
	vecEntities.push_back(entityClyde);
	mRegistry.emplace<CheckpointComponent>(entityClyde);
	iCheckpoints++;

	//power pill at the center
	std::random_device rd;
	std::mt19937 numGenerator(rd());
	std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
	entityPowerPill = factory.PillEntity(pAssetManager, maze->vecPowerPillVertices[distribution(numGenerator)], TileType::PowerPill, iMaxCols);
	vecEntities.push_back(entityPowerPill);
}
void EatGhosts_NoPills::Update(entt::registry& mRegistry, const float& fDeltaTime)
{
	fCurrentTime += fDeltaTime;
	if (fCurrentTime >= fTimer)
	{
		fCurrentTime -= fTimer;

		//respawn it with updated components if it isnt already present in the level
		if (!mRegistry.all_of<PillComponent>(entityPowerPill))
		{
			mRegistry.emplace<CollisionComponent>(entityPowerPill);
			mRegistry.emplace<PillComponent>(entityPowerPill, TileType::PowerPill);

			//randomize position for transformComponent of powerpill
			std::random_device rd;
			std::mt19937 numGenerator(rd());
			std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
			int index = distribution(numGenerator);
			auto gridPos = maze->vecPowerPillVertices[index];
			sf::Vector2f vPosition(gridPos.col * TileSize, gridPos.row * TileSize);

			mRegistry.patch<TransformComponent>(entityPowerPill, [gridPos, vPosition](auto& transformComp)
				{
					transformComp.vPosition = vPosition;
					transformComp.gridPos = gridPos;
				});

			mRegistry.patch<StaticSpriteComponent>(entityPowerPill, [vPosition](auto& spriteComp)
				{
					spriteComp.sprite.setPosition(vPosition);
					spriteComp.bDraw = true;
				});
		}
	}
}
void EatGhosts_NoPills::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string EatGhosts_NoPills::GetObjectiveString() const { return strObjective; }



//eat all 4 ghosts and fruits without ai
EatGhosts_Fruits::EatGhosts_Fruits(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Fruits and Ghosts";
}
void EatGhosts_Fruits::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//set timer to respawn powerpill at the center
	fTimer = 15.f;
	fCurrentTime = 0.f;

	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles

	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];
	
	//8 fruits without AI
	for (int i = 0; i < maze->vecGenericSpawnPoints.size(); i++)
	{
		auto e = factory.FruitEntity(pAssetManager, GetRandomizedFruitType(), maze->vecGenericSpawnPoints[i]);
		vecEntities.push_back(e);
		mRegistry.emplace<CheckpointComponent>(e);
		iCheckpoints++;
	}

	//spawn ghosts with checkpoints
	auto entityBlinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[0], maze->vecScatterCorners[0], GhostType::Blinky, Direction(1, 0));
	vecEntities.push_back(entityBlinky);
	mRegistry.emplace<CheckpointComponent>(entityBlinky);
	iCheckpoints++;

	auto entityInky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[1], maze->vecScatterCorners[1], GhostType::Inky, Direction(-1, 0));
	mRegistry.patch<GhostComponent>(entityInky, [entityBlinky](auto& ghostComp) {
		ghostComp.entityBlinky = entityBlinky;
		});
	vecEntities.push_back(entityInky);
	mRegistry.emplace<CheckpointComponent>(entityInky);
	iCheckpoints++;

	auto entityPinky = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[2], maze->vecScatterCorners[2], GhostType::Pinky, Direction(1, 0));
	vecEntities.push_back(entityPinky);
	mRegistry.emplace<CheckpointComponent>(entityPinky);
	iCheckpoints++;

	auto entityClyde = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGhostSpawnPoints[3], maze->vecScatterCorners[3], GhostType::Clyde, Direction(-1, 0));
	vecEntities.push_back(entityClyde);
	mRegistry.emplace<CheckpointComponent>(entityClyde);
	iCheckpoints++;

	//power pill at the center
	std::random_device rd;
	std::mt19937 numGenerator(rd());
	std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
	entityPowerPill = factory.PillEntity(pAssetManager, maze->vecPowerPillVertices[distribution(numGenerator)], TileType::PowerPill, iMaxCols);
	vecEntities.push_back(entityPowerPill);
}
void EatGhosts_Fruits::Update(entt::registry& mRegistry, const float& fDeltaTime)
{
	fCurrentTime += fDeltaTime;
	if (fCurrentTime >= fTimer)
	{
		fCurrentTime -= fTimer;

		//respawn it with updated components if it isnt already present in the level
		if (!mRegistry.all_of<PillComponent>(entityPowerPill))
		{
			mRegistry.emplace<CollisionComponent>(entityPowerPill);
			mRegistry.emplace<PillComponent>(entityPowerPill, TileType::PowerPill);

			//randomize position for transformComponent of powerpill
			std::random_device rd;
			std::mt19937 numGenerator(rd());
			std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
			int index = distribution(numGenerator);
			auto gridPos = maze->vecPowerPillVertices[index];
			sf::Vector2f vPosition(gridPos.col * TileSize, gridPos.row * TileSize);

			mRegistry.patch<TransformComponent>(entityPowerPill, [gridPos, vPosition](auto& transformComp)
				{
					transformComp.vPosition = vPosition;
					transformComp.gridPos = gridPos;
				});

			mRegistry.patch<StaticSpriteComponent>(entityPowerPill, [vPosition](auto& spriteComp)
				{
					spriteComp.sprite.setPosition(vPosition);
					spriteComp.bDraw = true;
				});
		}
	}
}
void EatGhosts_Fruits::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string EatGhosts_Fruits::GetObjectiveString() const { return strObjective; }



//eat 8 clydes
EatGhosts_Clydes::EatGhosts_Clydes(Maze* maze)
{
	this->maze = maze;
	this->strObjective = "Eat Ghosts";
}
void EatGhosts_Clydes::Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager)
{
	//set timer to respawn powerpill at the center
	fTimer = 15.f;
	fCurrentTime = 0.f;

	//load all entities like pills ghosts etc. according to the objective of the level
	EntityFactory factory(mRegistry);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / static_cast<int>(TileSize));					//max rows and cols of png texture tiles

	auto view = mRegistry.view<PacmanComponent>();
	auto entityPlayer = view[0];

	Direction direction(1, 0);
	for (int i = 0; i < maze->vecGenericSpawnPoints.size(); i++)
	{
		auto e = factory.GhostEntity(pAssetManager, entityPlayer, maze->vecGenericSpawnPoints[i], maze->vecScatterCorners[i / 2], GhostType::Clyde, direction);
		mRegistry.emplace<CheckpointComponent>(e);
		vecEntities.push_back(e);
		iCheckpoints++;
		direction *= -1;
	}

	//power pill at the center
	std::random_device rd;
	std::mt19937 numGenerator(rd());
	std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
	entityPowerPill = factory.PillEntity(pAssetManager, maze->vecPowerPillVertices[distribution(numGenerator)], TileType::PowerPill, iMaxCols);
	vecEntities.push_back(entityPowerPill);
}
void EatGhosts_Clydes::Update(entt::registry& mRegistry, const float& fDeltaTime)
{
	fCurrentTime += fDeltaTime;
	if (fCurrentTime >= fTimer)
	{
		fCurrentTime -= fTimer;
		
		//respawn it with updated components if it isnt already present in the level
		if (!mRegistry.all_of<PillComponent>(entityPowerPill))
		{
			mRegistry.emplace<CollisionComponent>(entityPowerPill);
			mRegistry.emplace<PillComponent>(entityPowerPill, TileType::PowerPill);

			//randomize position for transformComponent of powerpill
			std::random_device rd;
			std::mt19937 numGenerator(rd());
			std::uniform_int_distribution<int> distribution(0, maze->vecPowerPillVertices.size() - 1);
			int index = distribution(numGenerator);
			auto gridPos = maze->vecPowerPillVertices[index];
			sf::Vector2f vPosition(gridPos.col * TileSize, gridPos.row * TileSize);

			mRegistry.patch<TransformComponent>(entityPowerPill, [gridPos, vPosition](auto& transformComp)
				{
					transformComp.vPosition = vPosition;
					transformComp.gridPos = gridPos;
				});

			mRegistry.patch<StaticSpriteComponent>(entityPowerPill, [vPosition](auto& spriteComp)
				{
					spriteComp.sprite.setPosition(vPosition);
					spriteComp.bDraw = true;
				});
		}
	}
}
void EatGhosts_Clydes::Destroy(entt::registry& mRegistry) { Level::Destroy(mRegistry); }
std::string EatGhosts_Clydes::GetObjectiveString() const { return strObjective; }


