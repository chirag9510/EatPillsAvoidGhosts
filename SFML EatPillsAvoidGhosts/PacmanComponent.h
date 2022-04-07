#pragma once

struct PacmanComponent
{
	int iHealth;
	bool bInvulnerable;											//immune to ghosts and also doenst eats pills
	PacmanComponent() : iHealth(100), bInvulnerable(false) {}
};