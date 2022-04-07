#include "ChronoSystem.h"
#include "NavigationComponent.h"
#include "ChronoComponent.h"
#include "InputComponent.h"
#include "PacmanComponent.h"
#include "AnimationComponent.h"
#include "StaticSpriteComponent.h"
#include "TextComponent.h"

ChronoSystem::ChronoSystem(entt::registry& mRegistry) : mRegistry(mRegistry)
{}

void ChronoSystem::Update(const float& fDeltaTime)
{
	for (auto&& [e, chronoComp] : mRegistry.view<ChronoComponent>().each())
	{
		//only process if enabled
		if (chronoComp.chronoFncPtr != nullptr)
			//call the function pointer stored in timercomp for functionality
			(this->*chronoComp.chronoFncPtr)(e, fDeltaTime);
	}
}


void ChronoSystem::EnableMovement(const entt::entity& entity, const float& fDeltaTime)
{
	//enable movement after specified time
	auto& timerComp = mRegistry.get<TimerComponent>(entity);
	auto& currentTime = timerComp.fCurrentTime;
	currentTime += fDeltaTime;
	if (currentTime >= timerComp.fTimerLifeTime)
	{
		//remove timercomp
		currentTime = 0.f;

		//enable movement
		mRegistry.patch<NavigationComponent>(entity, [](auto& navComp) { navComp.bMove = true; });

		//also for player enable controls aswell
		if(mRegistry.all_of<InputComponent>(entity))
			mRegistry.patch<InputComponent>(entity, [](auto& inputComp) { inputComp.bEnableInput = true; });

		//null chrono funx pointer since theres no more use
		mRegistry.patch<ChronoComponent>(entity, [](auto& chronoComp)
			{
				chronoComp.chronoFncPtr = nullptr;
			});

		if (mRegistry.all_of<PacmanComponent>(entity))
		{
			mRegistry.patch<PacmanComponent>(entity, [](auto& pacmanComp)
				{
					pacmanComp.bInvulnerable = false;
				});
		}

		mRegistry.patch<StaticSpriteComponent>(entity, [](auto& spriteComp)
			{
				spriteComp.bDraw = true;
			});
	}
}

void ChronoSystem::Invulnerability(const entt::entity& entity, const float& fDeltaTime)
{
	//disable invulnerability after specified time
	auto& timerComp = mRegistry.get<TimerComponent>(entity);
	auto& currentTime = timerComp.fCurrentTime;
	currentTime += fDeltaTime;
	if (currentTime >= timerComp.fTimerLifeTime)
	{
		//remove timercomp
		currentTime = 0.f;

		//turn back to normal color
		mRegistry.patch<StaticSpriteComponent>(entity, [](auto& spriteComp)
			{
				spriteComp.sprite.setColor(sf::Color(255, 255, 255));
			});

		mRegistry.patch<PacmanComponent>(entity, [](auto& pacmanComp)
			{
				pacmanComp.bInvulnerable = false;
			});

		//null chrono funx pointer since theres no more use
		mRegistry.patch<ChronoComponent>(entity, [](auto& chronoComp)
			{
				chronoComp.chronoFncPtr = nullptr;
			});
	}
}

void ChronoSystem::Render(const entt::entity& entity, const float& fDeltaTime)
{
	//disable rendering after timer runs out
	auto& timerComp = mRegistry.get<TimerComponent>(entity);
	auto& currentTime = timerComp.fCurrentTime;
	currentTime += fDeltaTime;
	if (currentTime >= timerComp.fTimerLifeTime)
	{
		currentTime = 0.f;

		mRegistry.patch<StaticSpriteComponent>(entity, [](auto& spriteComp)
			{
				spriteComp.bDraw = false;
			});
		
		mRegistry.patch<ChronoComponent>(entity, [](auto& chronoComp)
			{
				chronoComp.chronoFncPtr = nullptr;
			});
	}
}

void ChronoSystem::FlashingRender(const entt::entity& entity, const float& fDeltaTime)
{
	//flashes the text indefinetly
	auto& timerComp = mRegistry.get<TimerComponent>(entity);
	timerComp.fCurrentTime += fDeltaTime;
	if (timerComp.fCurrentTime >= timerComp.fTimerLifeTime)
	{
		timerComp.fCurrentTime = 0.f;

		//flash sprite
		if (mRegistry.all_of<StaticSpriteComponent>(entity))
		{
			mRegistry.patch<StaticSpriteComponent>(entity, [](auto& spriteComp)
				{
					spriteComp.bDraw = !spriteComp.bDraw;
				});
		}
		else if (mRegistry.all_of<TextComponent>(entity))
		{
			mRegistry.patch<TextComponent>(entity, [](auto& textComp)
				{
					textComp.bDraw = !textComp.bDraw;
				});
		}
	}
}
