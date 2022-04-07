#pragma once
#include "Direction.h"

const enum InputState
{
	None,
	Up,
	Down,
	Left,
	Right
};


//change direction thru keyboard input
struct InputComponent
{
	bool bEnableInput;									//only change state in inputsystem when true							
	InputState state;
	InputComponent(InputState state = InputState::None) : bEnableInput(true), state(state) {}
};