#include "GameOverState.h"
#include "EntityFactory.h"
#include "ChronoComponent.h"
#include "PlayState.h"
#include <SFML/Window/Event.hpp>

GameOverState::GameOverState(sf::RenderWindow& mWindow, StateManager* stateManager, PlayerData& playerData) : 
	mWindow(mWindow), playerData(playerData), chronoSystem(mRegistry)
{
	this->stateManager = stateManager;
}

void GameOverState::OnEnter()
{
	LoadLevel();
	clockDelta.restart();
}

void GameOverState::LoadLevel()
{
	pAssetManager = std::make_unique<AssetManager>();
	pAssetManager->LoadFont("font", ".//assets//Emulogic.ttf");

	EntityFactory factory(mRegistry);
	sf::Vector2f vTextPosCenter(mWindow.getView().getSize().x / 2, mWindow.getView().getSize().y / 2);
	factory.TextLabelEntity(pAssetManager, "GAME OVER", vTextPosCenter, "font", 12, sf::Color::Red, TextAlign::Center);
	
	//update high score
	std::string strScore = "Score : " + std::to_string(playerData.iScore);
	sf::Color colorTxt = sf::Color::White;
	if (playerData.iScore > playerData.iHighScore)
	{
		//and flash in yellow colors
		strScore = "New High Score : " + std::to_string(playerData.iScore);
		colorTxt = sf::Color(255, 255, 50);
		playerData.iHighScore = playerData.iScore;
	}
	auto e = factory.TextLabelEntity(pAssetManager, strScore, sf::Vector2f(vTextPosCenter.x, vTextPosCenter.y + 20), "font", 6, colorTxt, TextAlign::Center);
	auto& chronoComp = mRegistry.emplace<ChronoComponent>(e);
	chronoComp.chronoFncPtr = &ChronoSystem::FlashingRender;
	auto& timerComp = mRegistry.emplace<TimerComponent>(e);
	timerComp.fTimerLifeTime = .5f;
	playerData.iScore = 0;

	factory.TextLabelEntity(pAssetManager, "[R] to Retry/[ESC] to Quit", sf::Vector2f(vTextPosCenter.x, vTextPosCenter.y + 54), "font", 6, sf::Color::White, TextAlign::Center);
}

void GameOverState::Run()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				stateManager->PopState();
				return;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				stateManager->ReplaceState<PlayState>();
				return;
			}
			
			break;
		case sf::Event::KeyReleased:

			break;
		}
	}

	float fDeltaTime = clockDelta.restart().asSeconds();
	//clamp deltaTime 
	fDeltaTime = fDeltaTime > 0.05f ? 0.05f : fDeltaTime;

	//update
	chronoSystem.Update(fDeltaTime);

	//draw
	renderSystem.Update(mRegistry, mWindow);
}

void GameOverState::OnReturn()
{}

void GameOverState::OnExit()
{
	mRegistry.clear();
	stateManager = nullptr;
}