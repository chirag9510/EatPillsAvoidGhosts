#include "AISystem.h"
#include "constants.h"
#include "NavigationComponent.h"
#include "TransformComponent.h" 
#include "AIComponent.h"
#include <ctime>
#include <random>

AISystem::AISystem(entt::registry& mRegistry, Maze& maze) : mRegistry(mRegistry), maze(maze), 
gridPosHomeCenter(13, 17), gridPosGate(13, 14)
{
	iYExitRow = 14;
	vecAllDirections.reserve(4);
	vecAllDirections.push_back(Direction(0, -1));			//up
	vecAllDirections.push_back(Direction(-1, 0));			//left
	vecAllDirections.push_back(Direction(0, 1));			//down
	vecAllDirections.push_back(Direction(1, 0));			//right
}

void AISystem::Update(const float& fDeltaTime)
{
	auto group = mRegistry.group<AIComponent, TransformComponent, NavigationComponent, GhostComponent>();
	for (auto e : group)
	{
		auto& navComp = group.get<NavigationComponent>(e);
		const auto& gridPos = group.get<TransformComponent>(e).gridPos;

		if (navComp.bMove)
		{
			Direction& direction = navComp.direction;
			float& fDistanceToCenter = navComp.fDistance;
			const float distance = navComp.fSpeed * fDeltaTime;
			fDistanceToCenter -= distance;
			if (fDistanceToCenter <= 0.f)
			{
				auto& behaviour = group.get<GhostComponent>(e).behaviour;
				auto& vPosition = group.get<TransformComponent>(e).vPosition;

				
				if (behaviour == Behaviour::Eaten && gridPos == gridPosHomeCenter)
				{
					//check if behavior was eaten and if it has then reached its home
					behaviour = Behaviour::Leaving;
					//turn it to regular speed
					navComp.fSpeed = maze.fSpeed_GhostNorm;

					navComp.direction = Direction(0, -1);
					navComp.bChangeDir = true;

					fDistanceToCenter += Distance_Leaving;
					
					ChangeAnimationWithDirection(mRegistry.get<AnimationComponent>(e), mRegistry.get<StaticSpriteComponent>(e), behaviour, direction);
					
					continue;
				}
				else if (behaviour == Behaviour::Leaving)
				{
					//if the overall maze behavior timer is still frighetened then ghost goes to chase behaviour.
					//and when the maze frightened timer is up it will go back to the real behaviour that the maze chooses
					if (maze.behaviourMaze != Behaviour::Frightened)
						behaviour = maze.behaviourMaze;
					else
						behaviour = Behaviour::Chase;
					
					navComp.directionNext = Direction(-1, 0);
					navComp.bChangeDir = true;

				}
			
				if (navComp.bChangeDir)
				{
					direction = navComp.directionNext;
					navComp.bChangeDir = false;

					//change the position according to direction
					vPosition = sf::Vector2f(
						(gridPos.col * TileSize + TileSize_Half) - (direction.X * fDistanceToCenter),
						(gridPos.row * TileSize + TileSize_Half) - (direction.Y * fDistanceToCenter));

					//change animation 
					if (mRegistry.all_of<AnimationComponent>(e) && behaviour != Behaviour::Frightened)
						ChangeAnimationWithDirection(mRegistry.get<AnimationComponent>(e), mRegistry.get<StaticSpriteComponent>(e), behaviour, direction);
				}

				//the gridvertex of the next node in the current direction
				GridVertex gvNextTile(gridPos.col + direction.X, gridPos.row + direction.Y);
				//update distance to center of next tile
				fDistanceToCenter = DistanceVector2f(vPosition, sf::Vector2f((gvNextTile.col * TileSize + TileSize_Half), (gvNextTile.row * TileSize + TileSize_Half)));

				//call the ai funx to get the target tile which varies on the behaviour
				auto& aiComp = group.get<AIComponent>(e);
				if (aiComp.aiFuncPtr != nullptr)
				{
					const GridVertex& gridPosTarget = (this->*aiComp.aiFuncPtr)(e);		//the target tile

					//***FRIGHTENED RANDOM DIRECTIONS
					//if ghost behaviour is frightened then they go in random directions except for reverse direction only once
					if (group.get<GhostComponent>(e).behaviour == Behaviour::Frightened)
					{
						std::vector<Direction> vecValidDirections;			//directions that dont lead to walls
						Direction dirNext = direction;
						for (auto dir : vecAllDirections)
						{
							GridVertex gvNeighbor(gvNextTile.col + dir.X, gvNextTile.row + dir.Y);

							//ignore tile in opposite direction and reverse direction isnt allowed
							if (!navComp.bReverseDir && direction == (dir * -1))
								continue;

							if (maze.setPathVertices.contains(gvNeighbor))
							{
								vecValidDirections.push_back(dir);
								dirNext = dir;
							}
						}

						if (vecValidDirections.size())
						{
							//choose random valid path tile since ghosts are frightened and panicking
							std::random_device rd;
							std::mt19937 numGenerator(rd());
							std::uniform_int_distribution<int> distribution(0, vecValidDirections.size() - 1);
							navComp.directionNext = vecValidDirections[distribution(numGenerator)];
							if (direction != dirNext)
								navComp.bChangeDir = true;
						}

						//only allow reverse direction for the first next tile
						navComp.bReverseDir = false;
					}
					//*** SCATTER/CHASE CANT GO IN REVERSE
					else
					{
						int iClosestDistance = 0, iDistance = 0;
						bool bGateValid = behaviour == Behaviour::Eaten || behaviour == Behaviour::Leaving;
						for (auto& dir : vecAllDirections)
						{
							//ignore tile in opposite direction if reverse direction isnt allowed
							if (!navComp.bReverseDir && direction == (dir * -1))
								continue;
							else if (navComp.zoneType == ZoneType::Non_Upwards && dir == Direction(0, -1))		//ignore tile in upwards directions if they are in the zone
								continue;

							GridVertex gvNeighbor(gvNextTile.col + dir.X, gvNextTile.row + dir.Y);

							//check if next tile isnt a wall
							if (maze.setPathVertices.contains(gvNeighbor) || (bGateValid && maze.setGateVertices.contains(gvNeighbor)))
							{
								iDistance = Distance(gvNeighbor, gridPosTarget);

								if (!iClosestDistance || iDistance < iClosestDistance)
								{
									//calculate the minimum distance
									iClosestDistance = iDistance;
									navComp.directionNext = dir;
									if (direction != dir)
										navComp.bChangeDir = true;
								}
							}
						}
						
						navComp.bReverseDir = false;
					}
				}
			}
		}
	}
}

GridVertex AISystem::BlinkyUpdate(const entt::entity& entity)
{
	auto& blinkyComp = mRegistry.get<GhostComponent>(entity);
	switch (blinkyComp.behaviour)
	{
	case Behaviour::Chase:
		//chase directly at player position
		return mRegistry.get<TransformComponent>(mRegistry.get<GhostComponent>(entity).entityPlayer).gridPos;
		
	//every ghost has the same target pos when eaten as they rush back home
	case Behaviour::Eaten:
		return gridPosHomeCenter;

	case Behaviour::Leaving:
		return gridPosGate;

	//default is scatter mode
	default:
	case Behaviour::Scatter:
		return blinkyComp.gridPosScatter;
	}	
}

GridVertex AISystem::PinkyUpdate(const entt::entity& entity)
{
	auto& pinkyComp = mRegistry.get<GhostComponent>(entity);
	switch (pinkyComp.behaviour)
	{
	case Behaviour::Chase:
	{
		auto entityPlayer = mRegistry.get<GhostComponent>(entity).entityPlayer;
		const auto& gridPosPlayer = mRegistry.get<TransformComponent>(entityPlayer).gridPos;
		const auto& directionPlayer = mRegistry.get<NavigationComponent>(entityPlayer).direction;
		return GridVertex(gridPosPlayer.col + (directionPlayer.X * 4), gridPosPlayer.row + (directionPlayer.Y * 4));
	}

	//every ghost has the same target pos when eaten as they rush back home
	case Behaviour::Eaten:
		return gridPosHomeCenter;


	case Behaviour::Leaving:
		return gridPosGate;
	
	default:
	case Behaviour::Scatter:
		return pinkyComp.gridPosScatter;
	}
}

GridVertex AISystem::InkyUpdate(const entt::entity& entity)
{
	auto& inkyComp = mRegistry.get<GhostComponent>(entity);
	switch (inkyComp.behaviour)
	{
	case Behaviour::Chase:
	{
		auto& entityBlinky = mRegistry.get<GhostComponent>(entity).entityBlinky;
		const auto gridPosBlinky = mRegistry.get<TransformComponent>(entityBlinky).gridPos;
		auto& entityPlayer = mRegistry.get<GhostComponent>(entity).entityPlayer;
		const auto gridPosPlayer = mRegistry.get<TransformComponent>(entityPlayer).gridPos;
		const auto directionPlayer = mRegistry.get<NavigationComponent>(entityPlayer).direction;
		//2 tile offset to player tile position
		GridVertex gridPosOffset(gridPosPlayer.col + (directionPlayer.X * 2), gridPosPlayer.row + (directionPlayer.Y * 2));

		return GridVertex(gridPosOffset.col + (gridPosOffset.col - gridPosBlinky.col), gridPosOffset.row + (gridPosOffset.row - gridPosBlinky.row));
	}

	case Behaviour::Eaten:
		return gridPosHomeCenter;


	case Behaviour::Leaving:
		return gridPosGate;

	default:
	case Behaviour::Scatter:
		return inkyComp.gridPosScatter;
	
	}
}

GridVertex AISystem::ClydeUpdate(const entt::entity& entity)
{
	auto& clydeComp = mRegistry.get<GhostComponent>(entity);
	switch (clydeComp.behaviour)
	{
	case Behaviour::Chase:
	{
		auto gridPosPlayer = mRegistry.get<TransformComponent>(clydeComp.entityPlayer).gridPos;
		//if the distance is more than 8 tiles then chase directly
		if (Distance(mRegistry.get<TransformComponent>(entity).gridPos, gridPosPlayer) > 8)
			return gridPosPlayer;
		else
			return clydeComp.gridPosScatter;								//otherwise return to the scatter corner
	}

	case Behaviour::Eaten:
		return gridPosHomeCenter;

	case Behaviour::Leaving:
		return gridPosGate;

	default:
	case Behaviour::Scatter:
		return clydeComp.gridPosScatter;

	}
}


void AISystem::ChangeAnimationWithDirection(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Behaviour behaviour, Direction direction)
{	
	//update animation vector from anim component
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

	animComp.fCurrentFrameTime = 0.f;
	animComp.iFrame = 0;

	spriteComp.sprite.setTextureRect(animComp.mapAnims[animComp.strAnim][0]);
}

float AISystem::DistanceVector2f(const sf::Vector2f& v1, const sf::Vector2f& v2) const
{
	float fX = v1.x - v2.x;
	float fY = v1.y - v2.y;
	return sqrt(fX * fX + fY * fY);
}

int AISystem::Distance(const GridVertex& gridPos, const GridVertex& gridPosTarget) const
{
	int iCol = gridPos.col - gridPosTarget.col;
	int iRow = gridPos.row - gridPosTarget.row;
 	return sqrt(iCol * iCol + iRow * iRow);
}
