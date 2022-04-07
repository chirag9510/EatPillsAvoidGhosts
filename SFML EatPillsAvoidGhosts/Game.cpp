#include "Game.h"
#include "constants.h"
#include <fstream>
#include <nlohmann/json.hpp>

Game::Game() : mStateManager(mWindow)
{}

void Game::Run() 
{
	Init();
	mStateManager.Run();			//run
	Destroy();
}

void Game::Init()
{
	GetWindowSizeFromFile();

	float viewW = static_cast<float>(TileRows) * TileSize, viewH = static_cast<float>(TileCols) * TileSize;
	sf::View view(sf::FloatRect(0.f, 0.f, viewW, viewH));
	view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

	mWindow.create(sf::VideoMode(viewW * fWindowSize, viewH * fWindowSize), "Eat Pills Avoid Ghosts", sf::Style::Titlebar);
	mWindow.setView(view);
	mWindow.setFramerateLimit(60);
	mWindow.setKeyRepeatEnabled(false);
}

void Game::GetWindowSizeFromFile()
{
	//read high score from the json file
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
			if (strName == "Object Layer")
			{
				for (const auto& value : jsonData["properties"])
				{
					if (value["name"] == "WindowSize")
					{
						fWindowSize = static_cast<float>(value["value"]);
						if (fWindowSize < 1.f)
							fWindowSize = 1.f;
						else if (fWindowSize > 8.f)
							fWindowSize = 8.f;
						break;
					}
				}
			}
		}

		fileData.close();
	}

}

void Game::Destroy()
{
	mStateManager.Destroy();
	mWindow.close();
}
