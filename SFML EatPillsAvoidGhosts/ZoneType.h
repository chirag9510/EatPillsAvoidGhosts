#pragma once

enum class ZoneType
{
	None,
	Slow,									//ghosts become slow
	Non_Upwards,								//ghosts cant go upwards
	Gate									//the ghost home at the center
};
