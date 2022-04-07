#pragma once
#include "Maze.h"
#include "AssetManager.h"
#include "FruitType.h"
#include <vector>
#include <entt/entity/registry.hpp>

class Maze;

class Level
{
protected:
	Maze* maze;
	std::string strObjective;
	float fTimer, fCurrentTime;							//timers for specific level characteristics
	std::vector<entt::entity> vecEntities;				//hold all entities from the current level and then clear them for the next one
	FruitType GetRandomizedFruitType();				

public:
	int iCheckpoints;									//if 0 then level is finished, load the next one. Checkpoints can be pills or ghosts depending on the objective of the level

	Level();
	virtual ~Level() = default;
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) = 0;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) = 0;
	virtual void Destroy(entt::registry& mRegistry);
	virtual std::string GetObjectiveString() const = 0;	//get the objective string displayed on the gameplay window
	bool HasCheckpoints();								//if true then the objective is not yet complete(i.e. some pills are still left to be eaten). if false then objective is complete and load the next level
};

//plain classic
class Classic : public Level
{
public:
	Classic(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;
};

//classic with no PowerPills
class Classic_NoPowerPills : public Level
{
public:
	Classic_NoPowerPills(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//classic but all pills are powerpills
class Classic_PowerPillOnly : public Level
{
public:
	Classic_PowerPillOnly(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};


//classic 1 blinky 2 inkies
class Classic_BlinkyInky : public Level
{
public:
	Classic_BlinkyInky(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//class 4 Clydes
class Classic_Clydes : public Level
{
public:
	Classic_Clydes(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//classic all pinkies
class Classic_Pinkies : public Level
{
public:
	Classic_Pinkies(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//only bonus items 
class TreasureRoom : public Level
{
public:
	TreasureRoom(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//full treasure room with 3 clydes
class TreasureRoom_ClydesChase : public Level
{
public:
	TreasureRoom_ClydesChase(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//8 bonus items with clyde ai
class Treasure_ClydeAI : public Level
{
public:
	Treasure_ClydeAI(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//8 bonus items with clyde ai and also 3 clydes to chase pacman
class Treasure_ClydesChase : public Level
{
public:
	Treasure_ClydesChase(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};


//eat all 4 ghosts from eating a powerpill spawned every few seconds and all pills
class EatGhosts : public Level
{
private:
	entt::entity entityPowerPill;						//the pill that respawns every few seconds

public:
	EatGhosts(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};

//eat all 4 ghosts from eating a randomly placed powerpill spawned every few seconds 
class EatGhosts_NoPills : public Level
{
private:
	entt::entity entityPowerPill;						//the pill that respawns every few seconds

public:
	EatGhosts_NoPills(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;
};


//eat all 4 ghosts from eating a randomly placed powerpill spawned every few seconds and some fruits (without AI)
class EatGhosts_Fruits : public Level
{
private:
	entt::entity entityPowerPill;						//the pill that respawns every few seconds

public:
	EatGhosts_Fruits(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;
};


//eat 8 clydes
class EatGhosts_Clydes: public Level
{
private:
	entt::entity entityPowerPill;						//the pill that respawns every few seconds

public:
	EatGhosts_Clydes(Maze* maze);
	virtual void Load(entt::registry& mRegistry, std::unique_ptr<AssetManager>& pAssetManager) override;
	virtual void Update(entt::registry& mRegistry, const float& fDeltaTime) override;
	virtual void Destroy(entt::registry& mRegistry) override;
	virtual std::string GetObjectiveString() const override;

};