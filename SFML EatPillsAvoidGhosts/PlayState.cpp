#include "PlayState.h"
#include "constants.h"
#include "EntityFactory.h"
#include "TileComponent.h"
#include "StaticSpriteComponent.h"
#include "AIComponent.h"
#include "PillComponent.h"
#include "TransformComponent.h"
#include "Maze.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "RespawnComponent.h"
#include "ChronoComponent.h"
#include "UIComponent.h"
#include "GameOverState.h"
#include "MainMenuState.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

PlayState::PlayState(sf::RenderWindow& mWindow, StateManager* stateManager, PlayerData& playerData) :
	mWindow(mWindow), navSystem(mRegistry, maze), aiSystem(mRegistry, maze), chronoSystem(mRegistry), maze(mRegistry, audioManager), bkgColorSystem(mRegistry),
	playerData(playerData), colSystem(mRegistry, maze, audioManager, playerData), animationSystem(mRegistry),
	CurrentStage(StageState::Start), fCurrentTime(0.f)
{
	this->stateManager = stateManager;
}

void PlayState::OnEnter()
{
	LoadLevel();

	//load maze level
	maze.ResetLevel(pAssetManager);

	//randomize level color
	bkgColorSystem.Init();

	clockDelta.restart();
}

void PlayState::LoadLevel()
{
	pAssetManager = std::make_unique<AssetManager>();
	//load assets
	pAssetManager->LoadFont("font", ".//assets//PressStart2P-vaV7.ttf");
	pAssetManager->LoadTexture("tileset", ".//assets//tileset.png");
	pAssetManager->LoadTexture("pacman", ".//assets//pacman.png");
	pAssetManager->LoadTexture("ghosts", ".//assets//ghosts.png");
	pAssetManager->LoadTexture("fruits", ".//assets//fruits.png");
	pAssetManager->LoadTexture("score", ".//assets//score.png");

	audioManager.LoadSoundBuffer("eat_fruit", ".//assets//eat_fruit.wav");
	audioManager.LoadSoundBuffer("eat_ghost", ".//assets//eat_ghost.wav");
	audioManager.LoadSoundBuffer("eat_pill", ".//assets//eat_pill.wav");
	audioManager.LoadSoundBuffer("pacman_hurt", ".//assets//pacman_hurt.wav");

	audioManager.LoadMusicPath("game_start", ".//assets//game_start.wav");
	audioManager.LoadMusicPath("game_start_2", ".//assets//game_start_2.wav");
	audioManager.LoadMusicPath("pacman_death", ".//assets//pacman_death.wav");
	audioManager.LoadMusicPath("powerpill_active", ".//assets//powerpill_active.wav");

	//read and load up lua file
	ReadMazeFile();
	LoadMiscEntities();
}

void PlayState::Run()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				if (CurrentStage == StageState::Paused)
				{
					CurrentStage = StageState::Play;
					
					//enable paused label
					mRegistry.patch<TextComponent>(entityPaused, [](auto& textComp) {
						textComp.bDraw = false;
						});

					//disable objective text label
					mRegistry.patch<TextComponent>(entityStageObj, [](auto& textComp) {
						textComp.bDraw = true;
						});

					//play sounds
					audioManager.ResumeMusic();
					audioManager.ResumeSound();

				}
				else if (CurrentStage == StageState::Play)
				{
					CurrentStage = StageState::Paused;

					//disable paused label
					mRegistry.patch<TextComponent>(entityPaused, [](auto& textComp) {
						textComp.bDraw = true;
						});

					//enable objective text label
					mRegistry.patch<TextComponent>(entityStageObj, [](auto& textComp) {
						textComp.bDraw = false;
						});

					//pause the sounds 
					audioManager.PauseMusic();
					audioManager.PauseSound();
				}
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && CurrentStage == StageState::Paused)
			{
				stateManager->PopState();
				return;
			}
			// check for chase and scatter behaviours on the fly
			//else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
			//{
			//	//switch scatter and chase
			//	for (auto&& [e, aiComp] : mRegistry.view<AIComponent>().each())
			//	{
			//		if (mRegistry.all_of<GhostComponent>(e))
			//		{
			//			auto& comp = mRegistry.get<GhostComponent>(e);
			//			if (comp.behaviour == Behaviour::Scatter)
			//				comp.behaviour = Behaviour::Chase;
			//			else
			//				comp.behaviour = Behaviour::Scatter;
			//		}
			//	}
			//}

			inputSystem.KeyPressed(event, mRegistry);

			break;

		case sf::Event::KeyReleased:
			inputSystem.KeyReleased(mRegistry);

			break;
		}
	}

	float fDeltaTime = clockDelta.restart().asSeconds();
	//clamp deltaTime 
	fDeltaTime = fDeltaTime > 0.05f ? 0.05f : fDeltaTime;

	switch (CurrentStage)
	{
	case StageState::Start:
		//play game after the music ends
	 	if (!audioManager.IsPlayingMusic())
		{
			CurrentStage = StageState::Play;
			//disable stage num text label (enabled by maze class when loading)
			mRegistry.patch<TextComponent>(entityStageNum, [](auto& textComp) {
				textComp.bDraw = false;
				});

			//and enable objective label
			mRegistry.patch<TextComponent>(entityStageObj, [](auto& textComp) {
				textComp.bDraw = true;
				});

			//and start animation on pacman
			mRegistry.patch<AnimationComponent>(entityPlayer, [](auto& animComp)
				{
					animComp.bAnimate = true;
				});


		}
		break;

	case StageState::Play:
	{
		//update
		chronoSystem.Update(fDeltaTime);
		navSystem.Update(fDeltaTime);
		aiSystem.Update(fDeltaTime);
		movementSystem.Update(mRegistry, fDeltaTime);
		colSystem.Update();
		maze.Update(fDeltaTime);

		//check win/lose condition
		if (mRegistry.get<PacmanComponent>(entityPlayer).iHealth <= 0)
		{
			CurrentStage = StageState::Over_PacmanDeath;
			//change pacman anim to death animation
			mRegistry.patch<AnimationComponent>(entityPlayer, [](auto& animComp)
				{
					animComp.strAnim = "death";
					animComp.fCurrentFrameTime = 0.f;
					animComp.iFrame = 0;
					animComp.iInc = 1;
					animComp.fAnimTime = .2f;
					animComp.bRepeat = false;
					animComp.bAnimate = true;
				});

			//disable objective text
			mRegistry.patch<TextComponent>(entityStageObj, [](auto& textComp)
				{
					textComp.bDraw = false;
				});

			audioManager.PlayMusic("pacman_death");

			//Destroy level
			maze.DestroyLevel();
		}
		else if (!maze.HasCheckpoints())
		{
			fCurrentTime = 0.f;
			CurrentStage = StageState::Next;

			audioManager.StopMusic();

			//destroy level to clear stuff 
			maze.DestroyLevel();
		}
	}
	break;

	case StageState::Paused:

		break;

	case StageState::Next:
	{
		fCurrentTime += fDeltaTime;
		if (fCurrentTime >= 3.f)
		{
			fCurrentTime = 0.f;
			
			//reset level
			maze.ResetLevel(pAssetManager);
			CurrentStage = StageState::Start;
		}
		
	}
	break;

	case StageState::Over_PacmanDeath:
		if (!audioManager.IsPlayingMusic())
		{
			CurrentStage = StageState::Over;

			//render game over text
			mRegistry.patch<TextComponent>(entityGameOverTxt, [](auto& textComp)
				{
					textComp.bDraw = true;
				});
		}

		break;

	case StageState::Over:
		//3 seconds of displaying game over later go to game over state
		fCurrentTime += fDeltaTime;
		if (fCurrentTime >= 4.f)
			stateManager->ReplaceState<GameOverState>();

		return;
	}

	animationSystem.Update(fDeltaTime);
	bkgColorSystem.Update(fDeltaTime);

	//draw and animate
	renderSystem.Update(mRegistry, mWindow);
}


// Read lua file and load tilemap
void PlayState::ReadMazeFile()
{
	std::ifstream fileData(".//assets//maze.json");
	if (fileData.is_open())
	{
		nlohmann::json jData;
		fileData >> jData;

		//read from layers array
		std::string strName;
		nlohmann::json jsonLayers = jData["layers"];
		for (const auto& jsonData : jsonLayers)
		{
			strName = jsonData["name"];
			if (strName == "Tile Layer")
			{
				LoadTiles(jsonData["data"]);
			}
			else if (strName == "Pill Layer")
			{
				LoadPills(jsonData["data"]);					//pills are on a seperate layer to tiles in json file
			}
			else if (strName == "Object Layer")
			{
				EntityFactory factory(mRegistry);
				for (const auto& data : jsonData["objects"])
				{
					strName = data["type"];
					if (strName == "spawnpoint")					//spawnpoint objects
					{
						strName = data["name"];
						if (strName == "pacman")
							entityPlayer = factory.PacmanEntity(pAssetManager, sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])));
					}
					else if (strName == "teleporttile")				//teleport tile 
					{
						factory.TeleportTileEntity(maze, sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])));
					}
					else if (strName == "ui")						//ui elements
					{
						strName = data["name"];
						if (strName == "score")
						{
							auto e = factory.TextLabelEntity(pAssetManager, "0", sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])), "font", 8, sf::Color::White);
							mRegistry.emplace<ScoreUIComponent>(e);
							factory.TextLabelEntity(pAssetManager, "Score", sf::Vector2f(static_cast<float>(data["x"]), 0), "font", 6, sf::Color(255, 255, 50));
						}
						else if (strName == "health")
						{
							auto e = factory.TextLabelEntity(pAssetManager, "100%", sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])), "font", 8, sf::Color::White);
							mRegistry.emplace<HealthUIComponent>(e);
							factory.TextLabelEntity(pAssetManager, "Health", sf::Vector2f(static_cast<float>(data["x"]), 0), "font", 6, sf::Color(255, 255, 50));
						}
						else if (strName == "stage_objective")
						{
							entityStageObj = factory.TextLabelEntity(pAssetManager, "None", sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])), "font", 5, sf::Color::White, TextAlign::Center);
							mRegistry.emplace<StageObjectiveUIComponent>(entityStageObj);
							//stage num
							entityStageNum = factory.TextLabelEntity(pAssetManager, "Stage 1", sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])), "font", 5, sf::Color(255, 255, 50), TextAlign::Center);
							mRegistry.emplace<StageNumUIComponent>(entityStageNum);
							//paused string
							entityPaused = factory.TextLabelEntity(pAssetManager, "Paused [ESC] to Resume/[Q] to Quit)", sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])), "font", 5, sf::Color(255, 255, 50), TextAlign::Center);
							mRegistry.emplace<PausedUIComponent>(entityPaused);
							mRegistry.patch<TextComponent>(entityPaused, [](auto& textComp) { textComp.bDraw = false; });
						}
					}
					else if (strName == "zone")
					{
						strName = data["name"];
						sf::IntRect rectZone(static_cast<int>(data["x"]), static_cast<int>(data["y"]), static_cast<int>(data["width"]), static_cast<int>(data["height"]));
						if (strName == "slow")
							factory.ZoneEntity(rectZone, ZoneType::Slow);
						else if (strName == "non_upwards")
							factory.ZoneEntity(rectZone, ZoneType::Non_Upwards);
						else if (strName == "gate")
							factory.ZoneEntity(rectZone, ZoneType::Gate);
					}
				}
			}
			else if (strName == "Spawns Layer")
			{
				for (const auto& data : jsonData["objects"])
				{
					auto it = maze.vecGhostSpawnPoints.begin();
					strName = data["type"];
					if (strName == "spawnpoint")
					{
						strName = data["name"];
						if (strName == "blinky")
							maze.vecGhostSpawnPoints.insert(it, sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])));
						else if (strName == "pinky")
							maze.vecGhostSpawnPoints.insert(it + 1, sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])));
						else if (strName == "inky")
							maze.vecGhostSpawnPoints.insert(it + 2, sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])));
						else if (strName == "clyde")
							maze.vecGhostSpawnPoints.insert(it + 3, sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])));
						else
							maze.vecGenericSpawnPoints.push_back(sf::Vector2f(static_cast<float>(data["x"]), static_cast<float>(data["y"])));
					}
				}
			}
		}

		fileData.close();
	}
	else
		std::cerr << "maze.json file not found" << std::endl;
}

void PlayState::LoadTiles(const std::vector<int>& vecData)
{
	EntityFactory factory(mRegistry);
	int row = 0, col = 0, iTileSize = static_cast<float>(TileSize);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / iTileSize);					//max rows and cols of png texture tiles

	for (const auto& i : vecData)
	{
		auto e = factory.TileEntity(pAssetManager, GridVertex(col, row));
		auto& spriteComp = mRegistry.get<StaticSpriteComponent>(e);
		spriteComp.sprite.setTextureRect(sf::IntRect(((i - 1) % iMaxCols) * iTileSize, ((i - 1) / iMaxCols) * iTileSize, iTileSize, iTileSize));
		spriteComp.sprite.setPosition(sf::Vector2f(col * TileSize, row * TileSize));

		TileType tileType = static_cast<TileType>(i - 1);

		//store all valid path nodes for pathfinding
		if (tileType == TileType::Path || tileType == TileType::PathSlow)
			maze.setPathVertices.insert(GridVertex(col, row));
		else if (tileType == TileType::Gate)
			maze.setGateVertices.insert(GridVertex(col, row));

		//for changing color by the system
		if (tileType != TileType::Gate && tileType != TileType::Pill && tileType != TileType::PowerPill)
			mRegistry.emplace<ColorTileComponent>(e);

		col++;
		if (col == TileRows)
		{
			col = 0;
			row++;
		}
	}
}

void PlayState::LoadPills(const std::vector<int>& vecData)
{
	EntityFactory factory(mRegistry);
	int row = 0, col = 0, iTileSize = static_cast<int>(TileSize);
	int iMaxCols = (pAssetManager->GetTexture("tileset").getSize().x / iTileSize);					//max rows and cols of png texture tiles
	TileType pillType = TileType::Pill;
	bool bSpawnTile = false;
	for (const auto& i : vecData)
	{
		pillType = static_cast<TileType>(i - 1);
		//store the grid pos of both types in maze class
		if (pillType == TileType::Pill)
		{
			maze.vecPillVertices.push_back(GridVertex(col, row));
			bSpawnTile = true;
		}
		else if (pillType == TileType::PowerPill)
		{
			maze.vecPowerPillVertices.push_back(GridVertex(col, row));
			bSpawnTile = true;
		}

		col++;
		if (col == TileRows)
		{
			col = 0;
			row++;
		}
	}
}

void PlayState::LoadMiscEntities()
{
	//load entities directly without json file
	EntityFactory factory(mRegistry);
	factory.BonusPointsEntity(pAssetManager, entityPlayer);

	//game over string
	sf::Vector2f vTextPosCenter(mWindow.getView().getSize().x / 2, mWindow.getView().getSize().y / 2);
	entityGameOverTxt = factory.TextLabelEntity(pAssetManager, "GAME OVER", vTextPosCenter, "font", 12, sf::Color::Red, TextAlign::Center);
	mRegistry.patch<TextComponent>(entityGameOverTxt, [](auto& textComp)
		{
			textComp.bDraw = false;
		});


}

GridVertex PlayState::ToGridVertex(const int X, const int Y)
{
	return GridVertex(X / TileSize, Y / TileSize);
}

void PlayState::OnReturn()
{}

void PlayState::OnExit()
{
	mRegistry.clear();
	stateManager = nullptr;
}

