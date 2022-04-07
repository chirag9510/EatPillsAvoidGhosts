#pragma once
#include <entt/entity/registry.hpp>

class ChronoSystem;

typedef void(ChronoSystem::*ChronoFncPtr)(const entt::entity& entity, const float& fDeltaTime);

class ChronoSystem
{
	entt::registry& mRegistry;

public:
	ChronoSystem(entt::registry& mRegistry);
	~ChronoSystem() = default;

	void Update(const float& fDeltaTime);

	//ChronoFnx associated with timers from TimerComponent, funx doing things when timer expires
	void EnableMovement(const entt::entity& entity, const float& fDeltaTime);				//enable movement
	void Invulnerability(const entt::entity& entity, const float& fDeltaTime);				//disable inv
	void Render(const entt::entity& entity, const float& fDeltaTime);						//Render as long as the timer works
	//indefinetly flashing sprite/text
	void FlashingRender(const entt::entity& entity, const float& fDeltaTime);
	//flashing text or sprite on screen for limited time 
	void FlashingRender_Timed(const entt::entity& entity, const float& fDeltaTime);
private:

};

