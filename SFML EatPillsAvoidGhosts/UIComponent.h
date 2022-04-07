#pragma once
#include <SFML/Graphics/Text.hpp>

//display score or other active text that isnt just a label
//the label "score" is just a plain label but the actual numerical score is a TextUIComponent
struct ScoreUIComponent
{};

//stage number
struct StageNumUIComponent
{};

//stage description
struct StageDescUIComponent
{};

struct HealthUIComponent
{};

//display objective string on the gameplay screen
struct StageObjectiveUIComponent
{};


//for displaying paused string when paused obviously
struct PausedUIComponent
{};