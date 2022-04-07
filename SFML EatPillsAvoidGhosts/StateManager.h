#pragma once
#include "GameState.h"
#include "PlayerData.h"
#include <stack>
#include <SFML/Graphics/RenderWindow.hpp>

class GameState;

class StateManager
{
private:
	PlayerData playerData;
	sf::RenderWindow& mWindow;
	std::stack<std::unique_ptr<GameState>> stkStates;

public:
	StateManager(sf::RenderWindow& mWindow);
	void Run();
	void Destroy();

	template <class TState>
	void PushState();
	template <class TState>
	void ReplaceState();

	void PopState();
};

template <class TState>
void StateManager::PushState()
{
	stkStates.push(std::make_unique<TState>(mWindow, this, playerData));
	stkStates.top()->OnEnter();							//run newly inserted state
}

template <class TState>
void StateManager::ReplaceState()
{
	PopState();
	PushState<TState>();
}


