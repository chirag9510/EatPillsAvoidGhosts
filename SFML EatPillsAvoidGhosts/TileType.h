#pragma once

//not only for components but also for reading maze file 
enum class TileType
{
	Wall,
	Path = 44,
	PathSlow = 50,										//path tile that slows down the ghosts
	Pill = 45,
	PowerPill = 47,
	Gate = 49											//gate of the pen where the ghosts live
};
