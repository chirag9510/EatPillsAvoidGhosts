#pragma once
#include "GameState.h"
#include "Maze.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "MovementSystem.h"
#include "NavigationSystem.h"
#include "CollisionSystem.h"
#include "AISystem.h"
#include "ChronoSystem.h"
#include "StageState.h"
#include "AnimationSystem.h"
#include "BackgroundColorSystem.h"
#include "AudioManager.h"
#include <SFML/System/Clock.hpp>

class PlayState : public GameState
{
private:
	StageState CurrentStage;
	PlayerData& playerData;
	sf::RenderWindow& mWindow;
	sf::Clock clockDelta;
	entt::entity entityPlayer, entityBlinky, entityStageNum, entityStageObj, entityGameOverTxt, entityPaused;
	float fCurrentTime;								//for timer


	//systems
	AudioManager audioManager;
	Maze maze;
	RenderSystem renderSystem;
	InputSystem inputSystem;
	MovementSystem movementSystem;
	NavigationSystem navSystem;
	CollisionSystem colSystem;
	ChronoSystem chronoSystem;
	AISystem aiSystem;
	AnimationSystem animationSystem;
	BackgroundColorSystem bkgColorSystem;

public:
	PlayState(sf::RenderWindow& mWindow, StateManager* stateManager, PlayerData& playerData);
	virtual void OnEnter() override;
	virtual void OnReturn() override;
	virtual void OnExit() override;

	virtual void Run() override;

private:
	void LoadLevel();

	void ReadMazeFile();
	void LoadTiles(const std::vector<int>& vecData);
	void LoadPills(const std::vector<int>& vecData);
	void LoadMiscEntities();

	GridVertex ToGridVertex(const int X, const int Y);
};

