#include "NavigationSystem.h"
#include "TransformComponent.h"
#include "InputComponent.h"
#include "constants.h"

NavigationSystem::NavigationSystem(entt::registry& mRegistry, Maze& maze) :  maze(maze), mRegistry(mRegistry)
{
}

void NavigationSystem::Update(const float& fDeltaTime)
{
	auto view = mRegistry.view<TransformComponent, NavigationComponent, InputComponent, AnimationComponent, StaticSpriteComponent>();
	for (auto [e, transformComp, navComp, inputComp, animComp, spriteComp] : view.each())
	{
		if (inputComp.state != InputState::None)
		{
			switch (inputComp.state)
			{
			case InputState::Up:
				ChangeDirection(navComp, animComp, spriteComp, transformComp.vPosition, transformComp.gridPos, Direction(0, -1));
				break;
			case InputState::Down:
				ChangeDirection(navComp, animComp, spriteComp, transformComp.vPosition, transformComp.gridPos, Direction(0, 1));

				break;
			case InputState::Right:
				ChangeDirection(navComp, animComp, spriteComp, transformComp.vPosition, transformComp.gridPos, Direction(1, 0));

				break;
			case InputState::Left:
				ChangeDirection(navComp, animComp, spriteComp, transformComp.vPosition, transformComp.gridPos, Direction(-1, 0));

				break;
			}
		}

		if (navComp.bChangeDir)
		{
			auto& direction = navComp.direction;
			const auto& directionNext = navComp.directionNext;

			//coming from either right or left side while cornering to turn
			if (directionNext.Y != 0)
			{
				if (direction.X > 0)											//if the cornering diagonal direction is towards right
				{
					if (static_cast<int>(transformComp.vPosition.x) >= navComp.vCenterLinePoint.x)
					{
						transformComp.vPosition.x = static_cast<float>(navComp.vCenterLinePoint.x);
						direction.X = 0;
						navComp.fDistance = 0.f;
						navComp.bChangeDir = false;
					}
				}
				else															//otherwise its towards left
				{
					if (static_cast<int>(transformComp.vPosition.x) <= navComp.vCenterLinePoint.x)
					{
						transformComp.vPosition.x = static_cast<float>(navComp.vCenterLinePoint.x);
						direction.X = 0;
						navComp.fDistance = 0.f;
						navComp.bChangeDir = false;
					}
				}
			}
			else
			{
				//coming from up or down
				if (direction.Y > 0)										//if the cornering diagonal direction is downwards
				{
					if (static_cast<int>(transformComp.vPosition.y) >= navComp.vCenterLinePoint.y)
					{
						transformComp.vPosition.y = static_cast<float>(navComp.vCenterLinePoint.y);
						direction.Y = 0;
						navComp.fDistance = 0.f;
						navComp.bChangeDir = false;
					}
				}
				else															//otherwise its towards left
				{
					if (static_cast<int>(transformComp.vPosition.y) <= navComp.vCenterLinePoint.y)
					{
						transformComp.vPosition.y = static_cast<float>(navComp.vCenterLinePoint.y);
						direction.Y = 0;
						navComp.fDistance = 0.f;
						navComp.bChangeDir = false;
					}
				}
			}

			//update animation
			ChangeAnimation(animComp, spriteComp, direction);
		}
		else if (navComp.bMove)
		{
			//just calculate distance b/w current pos and center pos of current tile
			//and when that distance become 0, check if next tile is a wall or a path
			//if its a wall then stop at the dead center otherwise calculate distance to center of next path tile and so on
			float& fDistanceToCenter = navComp.fDistance;
			fDistanceToCenter -= (navComp.fSpeed * fDeltaTime);
			if (fDistanceToCenter <= 0.f)
			{
				//check for walls while moving by processing the next tile in the same direction
				GridVertex gvNextTile(transformComp.gridPos.col + navComp.direction.X, transformComp.gridPos.row + navComp.direction.Y);
				//check if next tile isnt a wall
				if (maze.setPathVertices.contains(gvNextTile))
				{
					//if it is another path tile then set the distance and go on again
					fDistanceToCenter = Distance(transformComp.vPosition, sf::Vector2f(gvNextTile.col * TileSize + TileSize_Half, gvNextTile.row * TileSize + TileSize_Half));
				}
				else
				{
					//otherwise if the next tile is a wall then just stop
					transformComp.vPosition = sf::Vector2f(transformComp.gridPos.col * TileSize + TileSize_Half, transformComp.gridPos.row * TileSize + TileSize_Half);
					navComp.bMove = false;
					navComp.bBlocked = true;
					animComp.bAnimate = false;
				}
			}
		}

		

	}
}

void NavigationSystem::ChangeDirection(NavigationComponent& navComp, AnimationComponent& animComp, StaticSpriteComponent& spriteComp, const sf::Vector2f& vPosition, const GridVertex& gridPos, Direction moveDir)
{
	//first check if it isnt the same direction
	if (navComp.direction == moveDir)
		return;
	//else check if player just wants to go in the opposite direction
	else if (navComp.direction == (moveDir * -1))
	{
		navComp.direction = moveDir;
		navComp.bMove = true;

		ChangeAnimation(animComp, spriteComp, navComp.direction);
	}
	//otherwise check if the tile in which pacman wants to move is valid path or wall
	else if (maze.setPathVertices.contains(GridVertex(gridPos.col + moveDir.X, gridPos.row + moveDir.Y)) && !navComp.bChangeDir)
	{
		Direction& direction = navComp.direction;

		//if it has already stopped by a wall just change the direction and move
		if (!navComp.bMove && navComp.bBlocked)
		{
			direction = moveDir;
			navComp.bMove = true;

			ChangeAnimation(animComp, spriteComp, navComp.direction);
		}
		else
		{
			//cornering 
			//check whether pacman is pre or post turn the center point of the tile
			bool bPostTurn = false;
			navComp.vCenterLinePoint = sf::Vector2i(gridPos.col * TileSize + TileSize_Half, gridPos.row * TileSize + TileSize_Half);
			if (direction == Direction(0, -1))						//up
			{
				//post
				if (static_cast<int>(vPosition.y) < navComp.vCenterLinePoint.y)
					bPostTurn = true;
			}
			else if (direction == Direction(0, 1))					//down
			{
				if (static_cast<int>(vPosition.y) > navComp.vCenterLinePoint.y)
					bPostTurn = true;
			}
			else if (direction == Direction(-1, 0))					//left
			{
				if (static_cast<int>(vPosition.x) < navComp.vCenterLinePoint.x)
					bPostTurn = true;
			}
			else if (direction == Direction(1, 0))					//right
			{
				if (static_cast<int>(vPosition.x) > navComp.vCenterLinePoint.x)
					bPostTurn = true;
			}

			//if that is a path tile then change its course diagonally,
			//otherwise it wouldnt bother if the tile is a wall
			if (bPostTurn)
				navComp.direction *= -1;
			navComp.direction += moveDir;
			navComp.directionNext = moveDir;
			navComp.bChangeDir = true;
		}

		navComp.bBlocked = false;
	}
}


void NavigationSystem::ChangeAnimation(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Direction direction)
{
	animComp.fCurrentFrameTime = 0;
	animComp.iFrame = 0;
	animComp.iInc = 1;
	animComp.bAnimate = true;
	
	if (direction == Direction(0, -1))
		animComp.strAnim = "up";
	else if (direction == Direction(0, 1))
		animComp.strAnim = "down";
	else if (direction == Direction(1, 0))
		animComp.strAnim = "right";
	else if (direction == Direction(-1, 0))
		animComp.strAnim = "left";

	spriteComp.sprite.setTextureRect(animComp.mapAnims[animComp.strAnim][0]);
}


float NavigationSystem::Distance(sf::Vector2f v1, sf::Vector2f v2) const
{
	return sqrt(((v1.x - v2.x) * (v1.x - v2.x)) + ((v1.y - v2.y) * (v1.y - v2.y)));
}