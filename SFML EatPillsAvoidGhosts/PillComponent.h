#pragma once
#include "TileType.h"

//tags for pills
struct PillComponent
{
	TileType tileType;
	PillComponent(TileType tileType = TileType::Pill) : tileType(tileType) {}
};
