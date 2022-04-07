#pragma once

constexpr int SpriteSize = 16;								//sprite size for ghosts and pacman
constexpr int TileRows = 28;
constexpr int TileCols = 36;
constexpr float TileSize = 8.f;
constexpr float TileSize_Half = 4.f;

//misc
constexpr float DefaultSpeed = 77.f;
constexpr float GhostEatenSpeed = 132.f;
constexpr int PacmanMaxHealth = 100;

//ai behaviour timers
constexpr float Timer_Scatter = 7.f;
constexpr float Timer_Chase = 20.f;
constexpr float Timer_Frightened = 8.f;

//distance to gate when ghost behaviour changes from leaving to scatter/chase
constexpr float Distance_Leaving = 28.f;
