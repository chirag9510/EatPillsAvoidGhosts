#include "StateManager.h"
#include "MainMenuState.h"

StateManager::StateManager(sf::RenderWindow& mWindow) : mWindow(mWindow)
{

}

void StateManager::Run()
{
	//start with MainMenuState
	PushState<MainMenuState>();

	while (!stkStates.empty())
		stkStates.top()->Run();
}

void StateManager::Destroy()
{

}

void StateManager::PopState()
{
	stkStates.top()->OnExit();
	stkStates.top().reset();
	stkStates.pop();

	//call onReturn of the new top state if it exists
	if (!stkStates.empty())
		stkStates.top()->OnReturn();
}
