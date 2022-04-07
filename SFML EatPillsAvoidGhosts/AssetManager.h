#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <map>
#include <string>

class AssetManager
{
public:
	void LoadTexture(std::string strID, const char* szPath);
	sf::Texture& GetTexture(const std::string& strID);

	void LoadFont(std::string strID, const char* szPath);
	sf::Font& GetFont(const std::string& strID);

private:
	std::map<std::string, sf::Texture> mapTextures;
	std::map<std::string, sf::Font> mapFonts;
};

