//stores every node detail about the maze, useful for movement and pathfinding systems
#pragma once
#include "AssetManager.h"
#include "Level.h"
#include "GridVertex.h"
#include "GhostComponent.h"
#include "AnimationComponent.h"
#include "StaticSpriteComponent.h"
#include "AudioManager.h"
#include <entt/entity/registry.hpp>
#include <set>
#include <stack>

class Level;

class Maze
{
private:
	std::unique_ptr<Level> pLevel;										//current level
	entt::registry& mRegistry;
	AudioManager& audioManager;
	bool bReverseDir;									//for the ghosts when behaviour changes they can change the direction towards reverse for 1 tile
	int iLevel;											//every odd level is classic and even ones are randomized
	int iGhostPoints;									//starts from 100 and returns from 200 to 1600 with each ghost eaten

public:
	Maze(entt::registry& mRegistry, AudioManager& audioManager);
	~Maze() = default;

	std::set<GridVertex> setPathVertices;				//only valid PATH GridVertex vertices here, no walls. quick search to check for valid paths
	std::set<GridVertex> setGateVertices;				//gate of the pen the ghosts live in. becomes a valid path when ghosts are leaving the pen
	std::vector<GridVertex> vecPillVertices;				//GridVertex location of all pills. useful for respawning them for next level
	std::vector<GridVertex> vecPowerPillVertices;			//some levels spawn powerpills, some dont still useful to store thier location seperately from normal pills
	std::vector<GridVertex> vecScatterCorners;			//the corners where ghosts go to when in scatter mode 
	std::vector<sf::Vector2f> vecGhostSpawnPoints;		//seperate vector for easy identification
	std::vector<sf::Vector2f> vecGenericSpawnPoints;

	Behaviour behaviourMaze, behaviourPreviousMaze;								//current behaviour of the entire level
	float fTimer, fCurrentTime;							
	float fCurrentTime_Frightened;						//when frightned, chase/scatter timers are paused and this timer starts
	int inc, iMaxInc;									//increment for how many times behaviour can be switched from scatter to chase

	//speeds used by ghosts and pacman set according to the level
	float fSpeed_PacmanNorm, fSpeed_PacmanFright;		//pacman frightened and normal behaviour speed
	float fSpeed_GhostNorm, fSpeed_GhostFright, fSpeed_GhostTunnel;

	//update ai behaviour and other level timer stuff
	void Update(const float& fDeltaTime);
	void SetFrightenedBehaviorTimer();

	//load new level reset timers and increments
	void ResetLevel(std::unique_ptr<AssetManager>& pAssetManager);
	bool HasCheckpoints();
	void UpdateCheckpoint();			//update if player collides with a checkpoint be it pill or ghost depending on the objective of the level
	void DestroyLevel();

	int GetGhostPoints();				//return iGhost

private:
	void LoadLevel(std::unique_ptr<AssetManager>& pAssetManager);

	void AdjustEntitySpeed();
	void ResetPacmanPosition();

	void ChangeGhostBehaviour();
	void ChangeAnimationWithBehaviour(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Behaviour behaviour, Direction direction);
	
	void FillLevelShuffleBag();
};

