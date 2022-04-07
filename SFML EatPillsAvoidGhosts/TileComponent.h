#pragma once
#include "GridVertex.h"

//for static tiles only since they dont move and use transformComponent
struct TileComponent
{
	GridVertex gridPos;
	TileComponent(GridVertex gridPos = GridVertex(0,0)) : gridPos(gridPos) {}
};

struct TeleportTileComponent
{};

//for tiles whose colors are changed by backgroundColorsystem
struct ColorTileComponent 
{};