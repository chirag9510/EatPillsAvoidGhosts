#pragma once
#include <SFML/Graphics/Text.hpp>

enum class TextAlign
{
	None,
	Center,									//center entire text to the location
	Center_X								//only center the X value of the text not y
};

struct TextComponent
{
	bool bDraw;
	TextAlign align;
	sf::Text text;
	TextComponent(TextAlign align = TextAlign::None, bool bDraw = true) : bDraw(bDraw), align(align) {}
};