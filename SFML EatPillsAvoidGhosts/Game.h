#pragma once
#include "StateManager.h"

class Game
{
private:
	sf::RenderWindow mWindow;
	StateManager mStateManager;
	float fWindowSize;

public:
	Game();
	void Run();

private:
	void Init();
	void Destroy();							//quit

	void GetWindowSizeFromFile();
};

	

