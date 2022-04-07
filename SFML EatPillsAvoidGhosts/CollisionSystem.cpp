#include "CollisionSystem.h"
#include "PillComponent.h"
#include "TileComponent.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "StaticSpriteComponent.h"
#include "ChronoComponent.h"
#include "NavigationComponent.h"
#include "AIComponent.h"
#include "constants.h"
#include "UIComponent.h"
#include "TextComponent.h"
#include "InputComponent.h"
#include "ZoneComponent.h"
#include "PacmanComponent.h"
#include "CheckpointComponent.h"
#include "BonusPointsComponent.h"
#include "FruitComponent.h"

CollisionSystem::CollisionSystem(entt::registry& mRegistry, Maze& maze, AudioManager& audioManager, PlayerData& playerData)  :
	mRegistry(mRegistry), maze(maze), playerData(playerData), audioManager(audioManager)
{
	//Max score is unsigned int hence why -1 means max +ve value of int 
	MaxScore = -1;
}

void CollisionSystem::Update()
{
	//player  pill and ghost collisions
	auto groupPlayer = mRegistry.group<PacmanComponent>(entt::get<TransformComponent, StaticSpriteComponent, NavigationComponent, ChronoComponent, TimerComponent, AnimationComponent>);
	auto groupAI = mRegistry.group<AIComponent, GhostComponent>(entt::get<TransformComponent, NavigationComponent, StaticSpriteComponent, ChronoComponent, TimerComponent>);
	auto groupFruits = mRegistry.group<FruitComponent>(entt::get<TransformComponent>);
	for (auto entityPlayer : groupPlayer)
	{
		auto& pacmanComp = groupPlayer.get<PacmanComponent>(entityPlayer);
		auto gridPosPlayer = groupPlayer.get<TransformComponent>(entityPlayer).gridPos;

		//pill collision 
		if (!pacmanComp.bInvulnerable)
		{
			auto groupPills = mRegistry.group<PillComponent>(entt::get<TransformComponent, StaticSpriteComponent>);
			for (auto entityPill : groupPills)
			{
				if (groupPills.get<TransformComponent>(entityPill).gridPos == gridPosPlayer)
				{
					//update score and ai behavior
					PillCollision(entityPlayer, groupPills.get<PillComponent>(entityPill).tileType);

					//erase collision component and disable drawing
					mRegistry.erase<CollisionComponent>(entityPill);
					mRegistry.erase<PillComponent>(entityPill);
					auto& spriteComp = groupPills.get<StaticSpriteComponent>(entityPill);
					spriteComp.bDraw = false;

					//checkpointcomp update level
					if (mRegistry.all_of<CheckpointComponent>(entityPill))
						maze.UpdateCheckpoint();
				}
			}

			//ghost collision
			for (auto entityAI : groupAI)
			{
				//collision with player, only if hostile
				if (groupAI.get<GhostComponent>(entityAI).bHostile && gridPosPlayer == groupAI.get<TransformComponent>(entityAI).gridPos)
				{
					auto& aiBehaviour = groupAI.get<GhostComponent>(entityAI).behaviour;
					if (aiBehaviour == Behaviour::Frightened)
					{
						//if ghost has a checkpoint for the level EatAllGhosts, then disable them permanently instead of sending them to the house
						if (mRegistry.all_of<CheckpointComponent>(entityAI))
						{
							mRegistry.erase<GhostComponent>(entityAI);
							mRegistry.erase<CollisionComponent>(entityAI);
							mRegistry.erase<StaticSpriteComponent>(entityAI);
							maze.UpdateCheckpoint();
						}
						else
						{
							auto& navCompGhost = groupAI.get<NavigationComponent>(entityAI);
							//disable movement of ghost
							navCompGhost.bMove = false;
							aiBehaviour = Behaviour::Eaten;
							navCompGhost.fSpeed = GhostEatenSpeed;

							//now set timers for .5 sec which then enables movement
							groupAI.get<ChronoComponent>(entityAI).chronoFncPtr = &ChronoSystem::EnableMovement;
							groupAI.get<TimerComponent>(entityAI).fTimerLifeTime = .5f;
							
							//update if animcomp exists with the entity
							if (mRegistry.all_of<AnimationComponent>(entityAI))
								ChangeAnimationWithBehaviour(
									mRegistry.get<AnimationComponent>(entityAI),
									mRegistry.get<StaticSpriteComponent>(entityAI),
									aiBehaviour,
									navCompGhost.direction);

							groupAI.get<StaticSpriteComponent>(entityAI).bDraw = false;

						}

						//disable controls for pacman as well as movement
						groupPlayer.get<NavigationComponent>(entityPlayer).bMove = false;
						mRegistry.patch<InputComponent>(entityPlayer, [](auto& inputComp) 
							{
								inputComp.bEnableInput = false;
							});
						//now set timers for .5 sec which then enables movement
						groupPlayer.get<ChronoComponent>(entityPlayer).chronoFncPtr = &ChronoSystem::EnableMovement;
						groupPlayer.get<TimerComponent>(entityPlayer).fTimerLifeTime = .5f;
						groupPlayer.get<StaticSpriteComponent>(entityPlayer).bDraw = false;

						//make player invulnerable while it eats the ghost
						pacmanComp.bInvulnerable = true;
					
						//heal player 
						PlayerHeal(entityPlayer);

						//update score
						SetDrawScoreUI(groupPlayer.get<TransformComponent>(entityPlayer).vPosition, maze.GetGhostPoints());
					}
					else if (aiBehaviour == Behaviour::Chase || aiBehaviour == Behaviour::Scatter)
						PlayerDamageHealth(entityPlayer);			//update pacman health
				}
				
			}

			//fruit collision
			for (auto entityFruit : groupFruits)
			{
				//collision with player, only if hostile
				if (gridPosPlayer == groupFruits.get<TransformComponent>(entityFruit).gridPos)
				{
					//update score
					SetDrawScoreUI(groupPlayer.get<TransformComponent>(entityPlayer).vPosition, groupFruits.get<FruitComponent>(entityFruit).iScore);

					mRegistry.erase<FruitComponent>(entityFruit);
					mRegistry.erase<CollisionComponent>(entityFruit);
					mRegistry.erase<StaticSpriteComponent>(entityFruit);

					//checkpointcomp update level
					if (mRegistry.all_of<CheckpointComponent>(entityFruit))
						maze.UpdateCheckpoint();
				}
			}
		}
	}

	//and then ghost collision with a zone
	auto view = mRegistry.view<ZoneComponent>();
	for (auto entityAI : groupAI)
	{
		const auto vPosition = groupAI.get<TransformComponent>(entityAI).vPosition;
		auto& navComp = groupAI.get<NavigationComponent>(entityAI);
		for (auto [e, zoneComp] : view.each())
		{
			if (CheckZoneCollision(zoneComp.rectZone, vPosition))
			{
				//if ghost isnt in a zone already
				if (navComp.zoneType == ZoneType::None)
				{
					if (zoneComp.zoneType == ZoneType::Slow)
					{
						navComp.zoneType = ZoneType::Slow;
						if(groupAI.get<GhostComponent>(entityAI).behaviour != Behaviour::Eaten)			//ghosts in eaten state shouldnt slow down
							navComp.fSpeed = maze.fSpeed_GhostTunnel;
					}
					else if (zoneComp.zoneType == ZoneType::Non_Upwards)
					{
						navComp.zoneType = ZoneType::Non_Upwards;
					}
					else if (zoneComp.zoneType == ZoneType::Gate)
						navComp.zoneType = ZoneType::Gate;

					break;
				}
			}
			else if (navComp.zoneType != ZoneType::None)
			{
				//return speed to normal if it wasnt
				if (navComp.zoneType == ZoneType::Slow)
				{
					auto& behaviour = groupAI.get<GhostComponent>(entityAI).behaviour;
					if (groupAI.get<GhostComponent>(entityAI).behaviour == Behaviour::Frightened)
						navComp.fSpeed = maze.fSpeed_GhostFright;
					else if (behaviour == Behaviour::Eaten)
						navComp.fSpeed = GhostEatenSpeed;
					else 
						navComp.fSpeed = maze.fSpeed_GhostNorm;
				}

				//return back to normal
				navComp.zoneType = ZoneType::None;
			}
		}
	}


	//teleport tile collisions with pacman or ghosts
	auto groupTeleport = mRegistry.group<TeleportTileComponent>(entt::get<TransformComponent>);
	auto groupEntities = mRegistry.group<CollisionComponent>(entt::get<TransformComponent, NavigationComponent>);
	for(auto entityTeleport : groupTeleport) 
	{
		auto gridPosTeleport = groupTeleport.get<TransformComponent>(entityTeleport).gridPos;
		//gather all collision and transformComponent entities except the teleport tiles of course
		for (auto entity : groupEntities)
		{
			//if gridpos of entity is similiar to either of the teleporters
			auto& gridPos = groupEntities.get<TransformComponent>(entity).gridPos;
			if (gridPosTeleport == gridPos)
			{
				auto& vPosition = groupEntities.get<TransformComponent>(entity).vPosition;
				auto& direction = groupEntities.get<NavigationComponent>(entity).direction;
				if (gridPosTeleport.col < 0 && direction == Direction(-1, 0))
				{
					vPosition.x = TileRows * TileSize;
					gridPos = TileRows;
				}
				else if (gridPosTeleport.col > 0 && direction == Direction(1, 0))
				{
					vPosition.x = -TileSize;
					gridPos.col = -1;
				}

				groupEntities.get<NavigationComponent>(entity).fDistance = 4.f;
			}
		}
	}
}

void CollisionSystem::SetDrawScoreUI(sf::Vector2f vPosition, int iScore)
{
	auto group = mRegistry.group<BonusPointsComponent, AnimationComponent, StaticSpriteComponent, TimerComponent, ChronoComponent>();
	for (auto e : group)
	{
		//set chronofunx to render the sprite for 4 seconds
		auto& timerComp = group.get<TimerComponent>(e);
		timerComp.fCurrentTime = 0.f;
		timerComp.fTimerLifeTime = 4.f;
		group.get<ChronoComponent>(e).chronoFncPtr = &ChronoSystem::Render;

		auto& animComp = group.get<AnimationComponent>(e);
		animComp.strAnim = std::to_string(iScore);
		
		auto& spriteComp = group.get<StaticSpriteComponent>(e);
		spriteComp.bDraw = true;
		spriteComp.sprite.setPosition(vPosition);
		spriteComp.sprite.setTextureRect(animComp.mapAnims[animComp.strAnim][0]);
	}

	//eat fruit sound
	audioManager.PlaySound("eat_fruit");

	//update the score label
	UpdateScore(iScore);
}

void CollisionSystem::UpdateScore(int iScore)
{
	//cap max score so after reaching the max value of unsigned int it doesnt becomes 0
	auto& playerScore = playerData.iScore;
	if (MaxScore - playerScore <= iScore)
		playerScore = MaxScore;
	else
		playerScore += iScore;

	auto group = mRegistry.group<ScoreUIComponent, TextComponent, TransformComponent>();
	for (auto e : group)
	{
		auto& text = mRegistry.patch<TextComponent>(e).text;
		text.setString(std::to_string(playerScore));

		//center_x align
		const auto vPosition = group.get<TransformComponent>(e).vPosition;
		int x = vPosition.x, y = vPosition.y;
		sf::FloatRect rectText = text.getGlobalBounds();
		x -= rectText.width / 2.f; 
		text.setPosition(x, y);
	}
}


void CollisionSystem::PillCollision(const entt::entity entityPlayer, const TileType pillType)
{
	//update score and ghosts bieng able to go in reverse
	if (pillType == TileType::Pill)
	{
		audioManager.PlaySound("eat_pill");
		UpdateScore(10);
	}
	else if (pillType == TileType::PowerPill)
	{
		audioManager.PlaySound("eat_powerpill");
		audioManager.PlayMusic("powerpill_active", true);
		UpdateScore(100);

		//update player speed
		mRegistry.get<NavigationComponent>(entityPlayer).fSpeed = maze.fSpeed_PacmanFright;

		//update ai to  frightened only if in scatter/chase mode
		maze.SetFrightenedBehaviorTimer();
		auto group = mRegistry.group<AIComponent, GhostComponent, NavigationComponent>();
		for (auto e : group)
		{
			auto& behaviour = group.get<GhostComponent>(e).behaviour;
			if (behaviour == Behaviour::Scatter || behaviour == Behaviour::Chase)
			{
				behaviour = Behaviour::Frightened;
				auto& navComp = group.get<NavigationComponent>(e);
				navComp.fSpeed = maze.fSpeed_GhostFright;
				navComp.bReverseDir = true;

				//update animation 
				ChangeAnimationWithBehaviour(
					mRegistry.get<AnimationComponent>(e),
					mRegistry.get<StaticSpriteComponent>(e),
					behaviour,
					navComp.direction);
			}
		}
	}	
}

void CollisionSystem::ChangeAnimationWithBehaviour(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Behaviour behaviour, Direction direction)
{
	if (behaviour == Behaviour::Frightened)
		animComp.strAnim = "frightened";
	else
	{
		if (behaviour == Behaviour::Eaten)
			animComp.strAnim = "eyes_";
		else
			animComp.strAnim = "ghost_";

		if (direction == Direction(0, -1))
			animComp.strAnim += "up";
		if (direction == Direction(0, 1))
			animComp.strAnim += "down";
		if (direction == Direction(1, 0))
			animComp.strAnim += "right";
		if (direction == Direction(-1, 0))
			animComp.strAnim += "left";
	}

	animComp.fCurrentFrameTime = 0.f;
	animComp.iFrame = 0;

	spriteComp.sprite.setTextureRect(animComp.mapAnims[animComp.strAnim][0]);
}


void CollisionSystem::PlayerDamageHealth(const entt::entity entityPlayer)
{
	auto& pacmanComp = mRegistry.get<PacmanComponent>(entityPlayer);
	pacmanComp.iHealth -= 50;
	if (pacmanComp.iHealth > 0)						
	{
		pacmanComp.bInvulnerable = true;

		//turn the sprite red
		mRegistry.patch<StaticSpriteComponent>(entityPlayer, [](auto& spriteComp)
			{
				spriteComp.sprite.setColor(sf::Color(255, 0, 0));
			});

		//activate invulnerability mode for pacman for 2 seconds
		mRegistry.patch<ChronoComponent>(entityPlayer, [](auto& chronoComp)
			{
				chronoComp.chronoFncPtr = &ChronoSystem::Invulnerability;
			});

		mRegistry.patch<TimerComponent>(entityPlayer, [](auto& timerComp)
			{
				//timer invulnerability
				timerComp.fTimerLifeTime = 4.f;
			});


		//pacman hurt sound
		audioManager.PlaySound("pacman_hurt");
	}

	//update ui
	UpdateHealthUI(pacmanComp.iHealth);
}

void CollisionSystem::PlayerHeal(const entt::entity entityPlayer)
{
	auto& iHealth = mRegistry.get<PacmanComponent>(entityPlayer).iHealth;
	if (iHealth < PacmanMaxHealth)
		iHealth += 5;

	//eat ghost sound
	audioManager.PlaySound("eat_ghost");

	//update ui
	UpdateHealthUI(iHealth);
}

void CollisionSystem::UpdateHealthUI(const int iHealth)
{
	auto group = mRegistry.group<HealthUIComponent, TextComponent, TransformComponent>();
	for (auto e : group)
	{
		auto& text = group.get<TextComponent>(e).text;

		sf::Uint8 color_GB = static_cast<sf::Uint8>((255.f * static_cast<float>(iHealth)) / 100.f);

		if (iHealth < 0)
		{
			text.setString("0%");
			color_GB = 0;
		}
		else
			text.setString(std::to_string(iHealth) + "%");

		//set color of text in accordance with the amount of health
		text.setFillColor(sf::Color(255, color_GB, color_GB));

		//center_x align
		const auto vPosition = group.get<TransformComponent>(e).vPosition;
		int x = vPosition.x, y = vPosition.y;
		sf::FloatRect rectText = text.getGlobalBounds();
		x -= rectText.width / 2.f;
		text.setPosition(x, y);
	}
}

bool CollisionSystem::CheckZoneCollision(const sf::IntRect& rectZone, const sf::Vector2f& vPosition)
{
	return (vPosition.x >= rectZone.left && vPosition.x <= rectZone.left + rectZone.width
		&& vPosition.y >= rectZone.top && vPosition.y <= rectZone.top + rectZone.height);
}