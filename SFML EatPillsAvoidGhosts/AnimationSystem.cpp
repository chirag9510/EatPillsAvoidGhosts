#include "AnimationSystem.h"
#include "AnimationComponent.h"
#include "StaticSpriteComponent.h"

AnimationSystem::AnimationSystem(entt::registry& mRegistry) : mRegistry(mRegistry), time(0)
{
}

void AnimationSystem::Update(const float& fDeltaTime)
{
	auto group = mRegistry.group<AnimationComponent, StaticSpriteComponent>();
	for (auto e : group)
	{
		auto& animComp = group.get<AnimationComponent>(e);
		if (animComp.bAnimate)
		{
			auto& fCurrentFrameTime = animComp.fCurrentFrameTime;
			fCurrentFrameTime += fDeltaTime;
			if (fCurrentFrameTime >= animComp.fAnimTime)
			{
				fCurrentFrameTime = 0.f;
				auto& iFrame = animComp.iFrame;
				animComp.iFrame += animComp.iInc;
				if (iFrame >= animComp.mapAnims[animComp.strAnim].size() || iFrame < 0)
				{
					if (animComp.bRepeat)
					{
						if (animComp.bReverseAnim)
						{
							animComp.iInc *= -1;
							animComp.iFrame += (animComp.iInc * 2);
						}
						else
							animComp.iFrame = 0;
					}
					else
					{
						group.get<StaticSpriteComponent>(e).bDraw = false;
						animComp.bAnimate = false;
					}
				}

				//update sprite rect
				group.get<StaticSpriteComponent>(e).sprite.setTextureRect(animComp.mapAnims[animComp.strAnim][iFrame]);
			}
		}
	}
}