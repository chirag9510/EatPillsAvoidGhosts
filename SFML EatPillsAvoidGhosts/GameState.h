//base abstract GameState class
#pragma once
#include "AssetManager.h"
#include "StateManager.h"
#include <entt/entity/registry.hpp>

class StateManager;

class GameState
{
protected:
	StateManager* stateManager;
	entt::registry mRegistry;
	std::unique_ptr<AssetManager> pAssetManager;

public:
	virtual ~GameState() = default;
	virtual void OnEnter() = 0;							//when newly loaded into the stack 
	virtual void OnReturn() = 0;						//when the top state pops and this becomes the top on the stack, used to update certain values taken from the game (main menu uses this to update the displayed highscore)
	virtual void OnExit() = 0;							//pop() and destroy everything

	virtual void Run() = 0;
};

