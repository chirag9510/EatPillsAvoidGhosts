#pragma once
#include "Direction.h"
#include "GridVertex.h"
#include "ZoneType.h"
#include <SFML/System/Vector2.hpp>

struct NavigationComponent
{
	bool bMove;
	float fSpeed;				//the current speed
	Direction direction;		
	float fDistance;
	
	//when key is pressed, next direction to change is stored in directionNext
	//also direction is changed partially at a diagonal angle for cornering
	//when it reaches the centerpoint of the tile then change direction completely using directionNext
	//also used by ai for who only change their direction when they reach the centerpoint of the target tile
	bool bChangeDir;						//for pacman, change 
	sf::Vector2i vCenterLinePoint;			//the centerline point of current tile used when cornering
	Direction directionNext;				//the direction its going to take after cornering
	bool bBlocked;
	bool bReverseDir;						//ai use the reverse dir grid node for pathfinding if true. Used when switching from scatter, chase or frightned ai behaviour by collsion and ai system
	
	//for ai only, used to determine if they are in a zone in collisionsystem
	//if it isnt in any zone then it becomes ZoneType::None
	ZoneType zoneType;
//	float fSpeedPrevious;					//store the speed before entering the slow zone

	NavigationComponent(float fSpeed = 0.f, Direction direction = Direction(-1, 0))
		: direction(direction), bMove(true), bChangeDir(false)
		, fDistance(4.f), vCenterLinePoint(0, 0), directionNext(direction), bBlocked(false), bReverseDir(false)
		, zoneType(ZoneType::None), fSpeed(fSpeed)
	{}
};