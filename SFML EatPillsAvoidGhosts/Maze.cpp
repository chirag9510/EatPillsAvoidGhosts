#include "Maze.h"
#include "constants.h"
#include "NavigationComponent.h"
#include "ZoneComponent.h"
#include "TransformComponent.h"
#include "PacmanComponent.h"
#include "RespawnComponent.h"
#include "UIComponent.h"
#include "TextComponent.h"
#include "ChronoComponent.h"
#include "InputComponent.h"
#include <random>

Maze::Maze(entt::registry& mRegistry, AudioManager& audioManager) : mRegistry(mRegistry), audioManager(audioManager),
	inc(0), iMaxInc(7),
	fCurrentTime(0.f), fTimer(Timer_Scatter), 
	fCurrentTime_Frightened(0.f),
	behaviourMaze(Behaviour::Scatter), 
	bReverseDir(false),
	iLevel(0),
	iGhostPoints(100)
{
	vecGhostSpawnPoints.reserve(4);
	vecGenericSpawnPoints.reserve(8);
	pLevel = nullptr;

	vecScatterCorners.push_back(GridVertex(25, 0));
	vecScatterCorners.push_back(GridVertex(2, 0));
	vecScatterCorners.push_back(GridVertex(27, 35));
	vecScatterCorners.push_back(GridVertex(0, 35));
}

void Maze::Update(const float& fDeltaTime)
{
	if (behaviourMaze == Behaviour::Frightened)
	{
		fCurrentTime_Frightened += fDeltaTime;
		if (fCurrentTime_Frightened >= Timer_Frightened)
		{
			audioManager.StopMusic();

			behaviourMaze = behaviourPreviousMaze;
			bReverseDir = true;

			ChangeGhostBehaviour();
			
			//set back to 100 
			iGhostPoints = 100;
		}
	}
	else
	{
		fCurrentTime += fDeltaTime;
		if (fCurrentTime >= fTimer)
		{
			inc++;
			fCurrentTime = 0.f;

			//change behaviour after timer runs out
			if (behaviourMaze == Behaviour::Chase)
			{
				if (inc < iMaxInc)
				{
					if (inc > 3)
						fTimer = Timer_Scatter - 2.f;
					else
						fTimer = Timer_Scatter;

					behaviourMaze = Behaviour::Scatter;
					bReverseDir = true;

					ChangeGhostBehaviour();
				}
			}
			else if (behaviourMaze == Behaviour::Scatter)
			{
				fTimer = Timer_Chase;
				behaviourMaze = Behaviour::Chase;
				bReverseDir = true;

				ChangeGhostBehaviour();
			}
		}
	}


	//update level
	pLevel->Update(mRegistry, fDeltaTime);
}

void Maze::ChangeGhostBehaviour()
{
	//now apply this behaviour change in the ghosts
	auto group = mRegistry.group<GhostComponent>(entt::get<NavigationComponent>);
	for (auto e : group)
	{
		auto& behaviourGhost = group.get<GhostComponent>(e).behaviour;
		if (behaviourGhost != Behaviour::Eaten && behaviourGhost != Behaviour::Leaving)								//when its eaten, it must reach its home (maze center)
		{
			behaviourGhost = behaviourMaze;

			auto& navComp = group.get<NavigationComponent>(e);
			navComp.bReverseDir = bReverseDir;

			//only change velocity if ghost isnt in the tunnel
			if(navComp.zoneType != ZoneType::Slow)
				navComp.fSpeed = fSpeed_GhostNorm;

			if (mRegistry.all_of<AnimationComponent>(e))
				ChangeAnimationWithBehaviour(
					mRegistry.get<AnimationComponent>(e),
					mRegistry.get<StaticSpriteComponent>(e),
					behaviourGhost,
					navComp.direction);
		}
	}

	//update velocity of pacman. Only required when maze behaviour changes from frightened to
	auto groupPlayer = mRegistry.group<PacmanComponent>(entt::get<NavigationComponent>);
	for (auto e : groupPlayer)
		group.get<NavigationComponent>(e).fSpeed = fSpeed_PacmanNorm;
}

int Maze::GetGhostPoints()
{
	iGhostPoints = iGhostPoints * 2 > 1600 ? 1600 : iGhostPoints * 2;
	return iGhostPoints;
}

void Maze::SetFrightenedBehaviorTimer()
{
	if(behaviourMaze != Behaviour::Frightened)
		behaviourPreviousMaze = behaviourMaze;

	fCurrentTime_Frightened = 0.f;
	behaviourMaze = Behaviour::Frightened;
}

void Maze::ChangeAnimationWithBehaviour(AnimationComponent& animComp, StaticSpriteComponent& spriteComp, Behaviour behaviour, Direction direction)
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

void Maze::ResetLevel(std::unique_ptr<AssetManager>& pAssetManager)
{
	//reset timers
	inc = 0;
	fCurrentTime = 0.f;
	fTimer = Timer_Scatter;
	fCurrentTime_Frightened = 0.f;
	behaviourMaze = Behaviour::Scatter;
	bReverseDir = false;
	iGhostPoints = 100;

	DestroyLevel();
	LoadLevel(pAssetManager);
	ResetPacmanPosition();
	AdjustEntitySpeed();
}

void Maze::LoadLevel(std::unique_ptr<AssetManager>& pAssetManager)
{
	std::string strIntroMusic = "game_start";							//path to intro music. normal levels and eat ghosts levels use "game_start" whereas treasure levels use "game_start_2"

	//taking iLevel from 0 to 1 every odd level is classic while even levels are randomized with bonus ones
	iLevel++;
	if (iLevel % 2 == 0)
	{
		std::random_device rd;
		std::mt19937 numGenerator(rd());
		std::uniform_int_distribution<int> distribution(0, 12);
		switch (distribution(numGenerator))
		{
		case 0:
			pLevel = std::make_unique<Classic_NoPowerPills>(this);
			break;
		case 1:
			pLevel = std::make_unique<Classic_PowerPillOnly>(this);
			break;
		case 2:
			pLevel = std::make_unique<Classic_BlinkyInky>(this);
			break;
		case 3:
			pLevel = std::make_unique<Classic_Clydes>(this);
			break;
		case 4:
			pLevel = std::make_unique<Classic_Pinkies>(this);
			break;
		case 5:
			pLevel = std::make_unique<TreasureRoom>(this);
			strIntroMusic = "game_start_2";
			break;
		case 6:
			pLevel = std::make_unique<TreasureRoom_ClydesChase>(this);
			strIntroMusic = "game_start_2";
			break;
		case 7:
			pLevel = std::make_unique<Treasure_ClydeAI>(this);
			strIntroMusic = "game_start_2";
			break;
		case 8:
			pLevel = std::make_unique<Treasure_ClydesChase>(this);
			strIntroMusic = "game_start_2";
			break;
		case 9:
			pLevel = std::make_unique<EatGhosts>(this);
			break;
		case 10:
			pLevel = std::make_unique<EatGhosts_NoPills>(this);
			break;
		case 11:
			pLevel = std::make_unique<EatGhosts_Fruits>(this);
			break;
		case 12:
			pLevel = std::make_unique<EatGhosts_Clydes>(this);
			break;
		}
	}
	else
		pLevel = std::make_unique<Classic>(this);

	//load level
	pLevel->Load(mRegistry, pAssetManager);

	//set the objective text
	auto group = mRegistry.group<StageObjectiveUIComponent>(entt::get<TextComponent, TransformComponent>);
	for (auto e : group)
	{
		auto& textComp = group.get<TextComponent>(e);
		textComp.bDraw = false;
		auto& text = textComp.text;
		text.setString(pLevel->GetObjectiveString());

		//center align
		const auto vPosition = group.get<TransformComponent>(e).vPosition;
		int x = vPosition.x, y = vPosition.y;
		sf::FloatRect rectText = text.getGlobalBounds();
		x -= rectText.width / 2.f, y -= rectText.height / 2.f;
		text.setPosition(x, y);
	}

	//enable stage num text label
	auto groupStageNum = mRegistry.group<StageNumUIComponent>(entt::get<TextComponent, TransformComponent>);
	for (auto e : groupStageNum)
	{
		auto& textComp = groupStageNum.get<TextComponent>(e);
		textComp.bDraw = true;
		auto& text = textComp.text;
		text.setString("Stage " + std::to_string(iLevel));

		//center align
		const auto vPosition = groupStageNum.get<TransformComponent>(e).vPosition;
		int x = vPosition.x, y = vPosition.y;
		sf::FloatRect rectText = text.getGlobalBounds();
		x -= rectText.width / 2.f, y -= rectText.height / 2.f;
		text.setPosition(x, y);
	}

	//start the music
	audioManager.PlayMusic(strIntroMusic);
}

void Maze::AdjustEntitySpeed()
{
	//adjust velocity according to the level
	if (iLevel <= 2)
	{
		fSpeed_PacmanNorm = (80.f * DefaultSpeed) / 100.f;
		fSpeed_PacmanFright = (90.f * DefaultSpeed) / 100.f;
		fSpeed_GhostNorm = (75.f * DefaultSpeed) / 100.f;
		fSpeed_GhostFright = (50.f * DefaultSpeed) / 100.f;
		fSpeed_GhostTunnel = (40.f * DefaultSpeed) / 100.f;
	}
	else if (iLevel >= 3 && iLevel <= 6)
	{
		fSpeed_PacmanNorm = (90.f * DefaultSpeed) / 100.f;
		fSpeed_PacmanFright = (95.f * DefaultSpeed) / 100.f;
		fSpeed_GhostNorm = (85.f * DefaultSpeed) / 100.f;
		fSpeed_GhostFright = (55.f * DefaultSpeed) / 100.f;
		fSpeed_GhostTunnel = (45.f * DefaultSpeed) / 100.f;
	}
	else
	{
		fSpeed_PacmanNorm = (100.f * DefaultSpeed) / 100.f;
		fSpeed_PacmanFright = (100.f * DefaultSpeed) / 100.f;
		fSpeed_GhostNorm = (95.f * DefaultSpeed) / 100.f;
		fSpeed_GhostFright = (60.f * DefaultSpeed) / 100.f;
		fSpeed_GhostTunnel = (50.f * DefaultSpeed) / 100.f;
	}

	auto view = mRegistry.view<NavigationComponent>();
	for (auto [e, navComp] : view.each())
	{
		if (mRegistry.all_of<PacmanComponent>(e))
			navComp.fSpeed = fSpeed_PacmanNorm;
		else if (mRegistry.all_of<GhostComponent>(e))
			navComp.fSpeed = fSpeed_GhostNorm;
	}

}

void Maze::ResetPacmanPosition()
{
	//different characteristics can carryover from the previous level which need to be nullified back to thier default state
	auto group = mRegistry.group<PacmanComponent, TransformComponent, NavigationComponent, InputComponent, RespawnComponent, AnimationComponent, StaticSpriteComponent, ChronoComponent, TimerComponent>();
	for(auto e : group)
	{
		const auto vSpawnPos = group.get<RespawnComponent>(e).vSpawnPos;
		
		auto& transformComp = group.get<TransformComponent>(e);
		transformComp.vPosition = vSpawnPos;
		transformComp.gridPos = GridVertex(vSpawnPos.x / TileSize, vSpawnPos.y / TileSize);

		group.get<NavigationComponent>(e).direction = Direction(-1, 0);
		group.get<NavigationComponent>(e).bMove = true;

		auto& animComp = group.get<AnimationComponent>(e);
		animComp.strAnim = "left";
		animComp.fCurrentFrameTime = 0.f;
		animComp.iFrame = 0;
		animComp.iInc = 1;
		animComp.bAnimate = false;

		auto& spriteComp = group.get<StaticSpriteComponent>(e);
		spriteComp.bDraw = true;
		auto& sprite = spriteComp.sprite;
		sprite.setPosition(vSpawnPos);
		sprite.setTextureRect(animComp.mapAnims[animComp.strAnim][0]);

		//reset timercomp
		group.get<TimerComponent>(e).fCurrentTime = 0.f;

		//also enable controls aswell
		group.get<InputComponent>(e).bEnableInput = true;

		//null chrono funx pointer since theres no more use
		group.get<ChronoComponent>(e).chronoFncPtr = nullptr;

		//and disable invulnerabilities
		group.get<PacmanComponent>(e).bInvulnerable = false;
	}
}

void Maze::DestroyLevel()
{
	if (pLevel != nullptr)
	{
		pLevel->Destroy(mRegistry);
		pLevel.reset();
		pLevel = nullptr;
	}
}

void Maze::UpdateCheckpoint()
{
	pLevel->iCheckpoints--;
}

bool Maze::HasCheckpoints()
{
	return pLevel->HasCheckpoints();
}
