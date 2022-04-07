#include "MovementSystem.h"
#include "TransformComponent.h"
#include "NavigationComponent.h"
#include "StaticSpriteComponent.h"
#include "constants.h"

void MovementSystem::Update(entt::registry& mRegistry, const float& fDeltaTime)
{ 
	auto view = mRegistry.view<TransformComponent, NavigationComponent, StaticSpriteComponent>();
	view.each([fDeltaTime](const auto e, auto& transformComp, auto& navComp, auto& spriteComp)
		{
			if (navComp.bMove)
			{
				sf::Vector2f& vPosition = transformComp.vPosition;
				vPosition.x += navComp.direction.X * navComp.fSpeed * fDeltaTime;
				vPosition.y += navComp.direction.Y * navComp.fSpeed * fDeltaTime;

				//update gridPos
				transformComp.gridPos.col = vPosition.x / TileSize;
				transformComp.gridPos.row = vPosition.y / TileSize;

				//of course update the sprite location
				spriteComp.sprite.setPosition(vPosition);
			}
		});
}