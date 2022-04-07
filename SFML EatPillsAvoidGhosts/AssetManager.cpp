#include "AssetManager.h"
#include <iostream>

void AssetManager::LoadTexture(std::string strID, const char* szPath)
{
	if (!mapTextures[strID].loadFromFile(szPath))
		std::cerr << "LoadTexture() failed ID : " << strID << std::endl;
}

sf::Texture& AssetManager::GetTexture(const std::string& strID) 
{
	return mapTextures[strID];
}

void AssetManager::LoadFont(std::string strID, const char* szPath)
{
	if (!mapFonts[strID].loadFromFile(szPath))
		std::cerr << "LoadFont() failed ID : " << strID << std::endl;
}

sf::Font& AssetManager::GetFont(const std::string& strID)
{
	return mapFonts[strID];
}


