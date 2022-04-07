#pragma once
#include "GameState.h"
#include "AnimationSystem.h"
#include "RenderSystem.h"
#include "ChronoSystem.h"
#include "StageState.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <map>
#include <entt/entity/fwd.hpp>

class MainMenuState : public GameState
{
private:
	PlayerData& playerData;
	sf::RenderWindow& mWindow;
	sf::Clock clockDelta;
	entt::entity entityHighScore;

	//systems
	AnimationSystem animSystem;
	RenderSystem renderSystem;
	ChronoSystem chronoSystem;

public:
	MainMenuState(sf::RenderWindow& mWindow, StateManager* stateManager, PlayerData& playerData);
	virtual void OnEnter() override;
	virtual void OnReturn() override;
	virtual void OnExit() override;

	virtual void Run() override;

private:
	void LoadLevel();
};

