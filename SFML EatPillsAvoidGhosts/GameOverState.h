#pragma once
#include "GameState.h"
#include "PlayerData.h"
#include "StateManager.h"
#include "RenderSystem.h"
#include "ChronoSystem.h"
#include <SFML/Graphics/RenderWindow.hpp>

class GameOverState : public GameState
{
private:
	PlayerData& playerData;
	sf::RenderWindow& mWindow;
	sf::Clock clockDelta;

	//systems
	RenderSystem renderSystem;
	ChronoSystem chronoSystem;
public:
	GameOverState(sf::RenderWindow& mWindow, StateManager* stateManager, PlayerData& playerData);
	virtual void OnEnter() override;
	virtual void OnReturn() override;
	virtual void OnExit() override;

	virtual void Run() override;

private:
	void LoadLevel();
};

