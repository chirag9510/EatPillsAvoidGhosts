#pragma once
#include "Maze.h"
#include "PlayerData.h"
#include "TileType.h"
#include "PacmanComponent.h"
#include "AnimationComponent.h"
#include "StaticSpriteComponent.h"
#include "AudioManager.h"
#include <SFML/Graphics/Rect.hpp>
#include <entt/entity/registry.hpp>

//both pills and ghosts use pacman grid pos to determine collision
class CollisionSystem
{
	Maze& maze;
	entt::registry& mRegistry;
	PlayerData& playerData;
	AudioManager& audioManager;
	unsigned int MaxScore;

public:
	CollisionSystem(entt::registry& mRegistry, Maze& maze, AudioManager& audioManager, PlayerData& playerData);
	void Update();

private:
	//check AABB collision
	bool CheckZoneCollision(const sf::IntRect& rectZone, const sf::Vector2f& vPosition);

	//update score and change ai to frightened if neccasary
	void PillCollision(const entt::entity entityPlayer, const TileType pillType);

	//update animations of ghosts
	void ChangeAnimationWithBehaviour(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Behaviour behaviour, Direction direction);

	//update player health, iHealth is -2 when ghost hits it and powerpills(etc.) are +1 health
	void PlayerDamageHealth(const entt::entity entityPlayer);
	void PlayerHeal(const entt::entity entityPlayer);

	void UpdateHealthUI(const int iHealth);
	//render the score entitiy on the maze and update the overall score as well, called when pacman eats fruits or ghosts
	void SetDrawScoreUI(sf::Vector2f vPosition, int iScore);					
	void UpdateScore(int iScore);
};

