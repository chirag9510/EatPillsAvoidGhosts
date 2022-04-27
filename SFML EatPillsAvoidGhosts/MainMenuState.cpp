#include "MainMenuState.h"
#include "PlayState.h"
#include "EntityFactory.h"
#include "ChronoComponent.h"
#include "TransformComponent.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <fstream>
#include <nlohmann/json.hpp>


MainMenuState::MainMenuState(sf::RenderWindow& mWindow, StateManager* stateManager, PlayerData& playerData) : 
	mWindow(mWindow), playerData(playerData),
	animSystem(mRegistry), chronoSystem(mRegistry)
{
	this->stateManager = stateManager;
}

void MainMenuState::OnEnter()
{
	LoadLevel();
	clockDelta.restart();
}

void MainMenuState::LoadLevel()
{
	pAssetManager = std::make_unique<AssetManager>();
	pAssetManager->LoadFont("font", ".//assets//PressStart2P-vaV7.ttf");
	pAssetManager->LoadTexture("pacman", ".//assets//pacman.png");
	pAssetManager->LoadTexture("ghosts", ".//assets//ghosts.png");

	sf::Vector2f vViewSize = mWindow.getView().getSize();
	EntityFactory factory(mRegistry);

	//entities for StageState::MainMenu
	factory.TextLabelEntity(pAssetManager, "EAT PILLS", sf::Vector2f(vViewSize.x / 2, vViewSize.y / 4), "font", 12, sf::Color(255, 255, 50), TextAlign::Center);
	factory.TextLabelEntity(pAssetManager, "AVOID GHOSTS", sf::Vector2f(vViewSize.x / 2, vViewSize.y / 4 + 20), "font", 12, sf::Color(50, 50, 255), TextAlign::Center);
	
	auto e = factory.TextLabelEntity(pAssetManager, "Press [Enter] to Play", sf::Vector2f(vViewSize.x / 2, (vViewSize.y * 3) / 4 - 20), "font", 6, sf::Color::White, TextAlign::Center);
	auto& chronoComp = mRegistry.emplace<ChronoComponent>(e);
	chronoComp.chronoFncPtr = &ChronoSystem::FlashingRender;
	auto& timerComp = mRegistry.emplace<TimerComponent>(e);
	timerComp.fTimerLifeTime = .5f;

	//only display if high score is more than 0
	entityHighScore = factory.TextLabelEntity(pAssetManager, "HighScore " + std::to_string(playerData.iHighScore), sf::Vector2f(vViewSize.x / 2, vViewSize.y / 2.f), "font", 5, sf::Color(255, 255, 255), TextAlign::Center);
	mRegistry.patch<TextComponent>(entityHighScore, [](auto& textComp)
		{
			textComp.bDraw = false;
		});

	factory.TextLabelEntity(pAssetManager, "//chirag", sf::Vector2f(vViewSize.x / 2, vViewSize.y - 6), "font", 5, sf::Color(255, 255, 255), TextAlign::Center);

	auto entityPlayer = factory.PacmanEntity(pAssetManager, sf::Vector2f(vViewSize.x / 2.f - 36.f, vViewSize.y / 2.f - 20.f));
	factory.GhostEntity(pAssetManager, entityPlayer, sf::Vector2f(vViewSize.x / 2.f, vViewSize.y / 2.f - 20.f), GridVertex(0,0), GhostType::Blinky, Direction(-1, 0));
	factory.GhostEntity(pAssetManager, entityPlayer, sf::Vector2f(vViewSize.x / 2.f + 16.f, vViewSize.y / 2.f - 20.f), GridVertex(0,0), GhostType::Pinky, Direction(-1, 0));
	factory.GhostEntity(pAssetManager, entityPlayer, sf::Vector2f(vViewSize.x / 2.f + 32.f, vViewSize.y / 2.f - 20.f), GridVertex(0,0), GhostType::Inky, Direction(-1, 0));
	factory.GhostEntity(pAssetManager, entityPlayer, sf::Vector2f(vViewSize.x / 2.f + 48.f, vViewSize.y / 2.f - 20.f), GridVertex(0,0), GhostType::Clyde, Direction(-1, 0));

}

void MainMenuState::Run()
{
	//input
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
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))				//transition into play state
			{
				stateManager->PushState<PlayState>();
				return;
			}

			break;
		}
	}

	float fDeltaTime = clockDelta.restart().asSeconds();
	fDeltaTime = fDeltaTime > 0.05f ? 0.05f : fDeltaTime;

	//update
	chronoSystem.Update(fDeltaTime);
	animSystem.Update(fDeltaTime);
	
	//draw
	renderSystem.Update(mRegistry, mWindow);
}

void MainMenuState::OnReturn()
{
	if (playerData.iHighScore > 0)
	{
		int highScore = playerData.iHighScore;
		auto& textComp = mRegistry.get<TextComponent>(entityHighScore);

		textComp.text.setString("HighScore " + std::to_string(highScore));
		textComp.bDraw = true;

		//center_x align
		const auto vPosition = mRegistry.get<TransformComponent>(entityHighScore).vPosition;
		int x = vPosition.x, y = vPosition.y;
		sf::FloatRect rectText = textComp.text.getGlobalBounds();
		x -= rectText.width / 2.f;
		textComp.text.setPosition(x, y);
	}
}

void MainMenuState::OnExit()
{
	mRegistry.clear();
	stateManager = nullptr;
}