#include "InputSystem.h"
#include "NavigationComponent.h"
#include "Direction.h"
#include "InputComponent.h"

void InputSystem::KeyPressed(const sf::Event& event, entt::registry& mRegistry)
{
	auto view = mRegistry.view<InputComponent>();
	for (auto [entityPlayer, inputComp] : view.each())
	{
		if (inputComp.bEnableInput)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Up:
			case sf::Keyboard::W:
				inputComp.state = InputState::Up;
				break;

			case sf::Keyboard::Down:
			case sf::Keyboard::S:
				inputComp.state = InputState::Down;
				break;

			case sf::Keyboard::Right:
			case sf::Keyboard::D:
				inputComp.state = InputState::Right;
				break;

			case sf::Keyboard::Left:
			case sf::Keyboard::A:
				inputComp.state = InputState::Left;
				break;
			}
		}
	}
}

void InputSystem::KeyReleased(entt::registry& mRegistry)
{
	auto view = mRegistry.view<InputComponent>();
	for (auto [entityPlayer, inputComp] : view.each())
	{
		inputComp.state = InputState::None;
	}
}