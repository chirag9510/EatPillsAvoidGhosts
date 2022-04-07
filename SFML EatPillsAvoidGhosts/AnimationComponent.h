#pragma once
#include "Direction.h"
#include <map>
#include <string>
#include <vector>
#include <SFML/Graphics/Rect.hpp>

//character animation works with directions
//map contains anim type, direction and the vector for frame rects
//if ghosts are just chasing then anim type is chase, up/down/right/left, vector
//same for when they are eaten or frightened -> eaten, up/down/right/left, vector
//makes it easier for systems to just change the animation according to the character direction
//entities with a single animation use the AnimDirection "None"
struct AnimationComponent
{
	std::string strAnim;
	std::map<std::string, std::vector<sf::IntRect>> mapAnims;
	int iFrame;													//current frame
	float fAnimTime, fCurrentFrameTime;							//frame time
	bool bAnimate;						//only update frames if true
	bool bRepeat;						//repeat if true, otherwise set bDraw false after the animation is done

	//if true, then animation frame sequence goes in reverse order by making iInc -1 
	//then iInc becomes 1 again and frame sequence is in positive order
	//useful for pacman animations where the mouth opens and closes 
	bool bReverseAnim;													
	int iInc;													

	AnimationComponent(float fAnimTime = .2f) : 
		iInc(1), fAnimTime(fAnimTime), fCurrentFrameTime(0.f), bAnimate(true), bRepeat(true), bReverseAnim(false), iFrame(0), strAnim("") {}
};