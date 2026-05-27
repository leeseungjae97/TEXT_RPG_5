#include "GameInstance.h"

#include "Define.h"
#include "Manager/DisplayManager.h"
#include "Manager/BattleManager.h"
#include "Manager/ViewportManager.h"
#include "Manager/MapManager.h"
#include "Manager/TimeManager.h"
#include "Manager/SceneManager.h"
#include "Manager/InputManager.h"
#include "Manager/ShopManager.h"
#include "Player.h"

namespace
{
	string ToUtf8String(const wstring& Text)
	{
		if (Text.empty())
		{
			return "";
		}

		int length = WideCharToMultiByte(CP_UTF8, 0, Text.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (length <= 0)
		{
			string fallback;
			fallback.reserve(Text.length());
			for (wchar_t character : Text)
			{
				fallback.push_back(static_cast<char>(character));
			}
			return fallback;
		}

		string result(length - 1, '\0');
		WideCharToMultiByte(CP_UTF8, 0, Text.c_str(), -1, &result[0], length, nullptr, nullptr);
		return result;
	}
}

void GameInstance::BeginPlay()
{
	InputManager::GetInstance()->BeginPlay();
	DisplayManager::GetInstance()->BeginPlay();
	TimeManager::GetInstance()->BeginPlay();
	
	ShopManager::GetInstance()->BeginPlay();
	MapManager::GetInstance()->BeginPlay();
	SceneManager::GetInstance()->BeginPlay();
	BattleManager::GetInstance()->BeginPlay();
}

void GameInstance::Tick()
{
	TimeManager::GetInstance()->Tick();
	const float DeltaTime = static_cast<float>(TimeManager::GetInstance()->GetDeltaTime());
	
	InputManager::GetInstance()->Tick(DeltaTime);

	SceneManager::GetInstance()->Tick(DeltaTime);

	ViewportManager::GetInstance()->Tick(DeltaTime);
}
void GameInstance::Render()
{
	ViewportManager::GetInstance()->Render();
	DisplayManager::GetInstance()->Render();
}

void GameInstance::Destroy()
{
	SceneManager::GetInstance()->Destroy();
}

void GameInstance::Run()
{
	TimeManager::GetInstance()->Tick();
	const float DeltaTime = static_cast<float>(TimeManager::GetInstance()->GetDeltaTime());
	InputManager::GetInstance()->Tick(DeltaTime);

	switch (FlowState)
	{
	case GameFlowState::MainMenu:
		TickMainMenu(DeltaTime);
		MainMenu.Render();
		DisplayManager::GetInstance()->Render();
		return;

	case GameFlowState::NameInput:
		TickNameInput(DeltaTime);
		NameInput.Render();
		DisplayManager::GetInstance()->Render();
		return;

	case GameFlowState::FadeOutToGame:
		TickFadeOutToGame(DeltaTime);
		NameInput.Render();
		ViewportManager::GetInstance()->RenderFade();
		DisplayManager::GetInstance()->Render();
		return;

	case GameFlowState::FadeInGame:
		SceneManager::GetInstance()->Tick(DeltaTime);
		ViewportManager::GetInstance()->Tick(DeltaTime);
		TickFadeInGame(DeltaTime);
		ViewportManager::GetInstance()->Render();
		ViewportManager::GetInstance()->RenderFade();
		DisplayManager::GetInstance()->Render();
		Destroy();
		return;

	case GameFlowState::FadeOutToGameOver:
		TickFadeOutToGameOver(DeltaTime);
		ViewportManager::GetInstance()->Render();
		ViewportManager::GetInstance()->RenderFade();
		DisplayManager::GetInstance()->Render();
		Destroy();
		return;

	case GameFlowState::FadeInGameOver:
		TickFadeInGameOver(DeltaTime);
		GameOver.Render();
		ViewportManager::GetInstance()->RenderFade();
		DisplayManager::GetInstance()->Render();
		return;

	case GameFlowState::GameOver:
		TickGameOver(DeltaTime);
		GameOver.Render();
		DisplayManager::GetInstance()->Render();
		return;

	case GameFlowState::Game:
	default:
		SceneManager::GetInstance()->Tick(DeltaTime);
		if (Player* player = SceneManager::GetInstance()->GetPlayer())
		{
			if (player->IsDead())
			{
				FlowState = GameFlowState::FadeOutToGameOver;
				ViewportManager::GetInstance()->StartFadeOut();
				ViewportManager::GetInstance()->Render();
				ViewportManager::GetInstance()->RenderFade();
				DisplayManager::GetInstance()->Render();
				Destroy();
				return;
			}
		}
		ViewportManager::GetInstance()->Tick(DeltaTime);
		ViewportManager::GetInstance()->Render();
		DisplayManager::GetInstance()->Render();
		break;
	}

	Destroy();
}

void GameInstance::TickMainMenu(float DeltaTime)
{
	MainMenuAction action = MainMenu.Tick(DeltaTime);
	if (action == MainMenuAction::StartGame)
	{
		NameInput.Open();
		FlowState = GameFlowState::NameInput;
		return;
	}

	if (action == MainMenuAction::Exit)
	{
		RequestQuit();
	}
}

void GameInstance::TickNameInput(float DeltaTime)
{
	NameInputAction action = NameInput.Tick(DeltaTime);
	if (action == NameInputAction::Start)
	{
		if (Player* player = SceneManager::GetInstance()->GetPlayer())
		{
			player->SetName(ToUtf8String(NameInput.GetPlayerName()));
		}

		FlowState = GameFlowState::FadeOutToGame;
		ViewportManager::GetInstance()->StartFadeOut();
		return;
	}

	if (action == NameInputAction::Cancel)
	{
		FlowState = GameFlowState::MainMenu;
	}
}

void GameInstance::TickFadeOutToGame(float DeltaTime)
{
	ViewportManager::GetInstance()->TickFade(DeltaTime);
	if (ViewportManager::GetInstance()->IsFadeFinished())
	{
		FlowState = GameFlowState::FadeInGame;
		ViewportManager::GetInstance()->StartFadeIn();
	}
}

void GameInstance::TickFadeInGame(float DeltaTime)
{
	ViewportManager::GetInstance()->TickFade(DeltaTime);
	if (ViewportManager::GetInstance()->IsFadeFinished())
	{
		FlowState = GameFlowState::Game;
	}
}

void GameInstance::TickFadeOutToGameOver(float DeltaTime)
{
	ViewportManager::GetInstance()->TickFade(DeltaTime);
	if (ViewportManager::GetInstance()->IsFadeFinished())
	{
		FlowState = GameFlowState::FadeInGameOver;
		ViewportManager::GetInstance()->StartFadeIn();
	}
}

void GameInstance::TickFadeInGameOver(float DeltaTime)
{
	ViewportManager::GetInstance()->TickFade(DeltaTime);
	if (ViewportManager::GetInstance()->IsFadeFinished())
	{
		FlowState = GameFlowState::GameOver;
	}
}

void GameInstance::TickGameOver(float DeltaTime)
{
	GameOverAction action = GameOver.Tick(DeltaTime);
	if (action == GameOverAction::Retry)
	{
		ResetGameWorld();
		FlowState = GameFlowState::FadeInGame;
		ViewportManager::GetInstance()->StartFadeIn();
		return;
	}

	if (action == GameOverAction::MainMenu)
	{
		ResetGameWorld();
		FlowState = GameFlowState::MainMenu;
	}
}

void GameInstance::ResetGameWorld()
{
	ViewportManager::GetInstance()->ResetRuntimeCache();
	ShopManager::GetInstance()->ResetRuntimeCache();
	ShopManager::GetInstance()->RestoreShop();
	BattleManager::GetInstance()->Reset();
	SceneManager::GetInstance()->Reset();
	ViewportManager::GetInstance()->ResetRuntimeCache();
	
	RestartGame();
}

void GameInstance::RestartGame()
{
	ShopManager::GetInstance()->BeginPlay();
	MapManager::GetInstance()->BeginPlay();
	SceneManager::GetInstance()->BeginPlay();
	BattleManager::GetInstance()->BeginPlay();
}

GameInstance::GameInstance()
{
}

GameInstance::~GameInstance()
{
}
