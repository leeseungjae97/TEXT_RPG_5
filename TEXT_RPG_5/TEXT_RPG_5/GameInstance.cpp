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
#include "Manager/CraftingManager.h"
#include "Manager/EnhancementManager.h"
#include "Manager/ChestManager.h"
#include "Manager/StageManager.h"
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
	EnhancementManager::GetInstance()->BeginPlay();
	StageManager::GetInstance()->BeginPlay();
	ChestManager::GetInstance()->BeginPlay();
	MapManager::GetInstance()->BeginPlay();
	SceneManager::GetInstance()->BeginPlay();
	BattleManager::GetInstance()->BeginPlay();
}

void GameInstance::Tick()
{
	TimeManager::GetInstance()->Tick();
	const float DeltaTime = static_cast<float>(TimeManager::GetInstance()->GetDeltaTime());
	
	InputManager::GetInstance()->Tick(DeltaTime);
	StageManager::GetInstance()->Tick(DeltaTime);

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

	case GameFlowState::FadeOutToNextStage:
		TickFadeOutToNextStage(DeltaTime);
		ViewportManager::GetInstance()->Render();
		ViewportManager::GetInstance()->RenderFade();
		DisplayManager::GetInstance()->Render();
		Destroy();
		return;

	case GameFlowState::FadeInNextStage:
		SceneManager::GetInstance()->Tick(DeltaTime);
		ViewportManager::GetInstance()->Tick(DeltaTime);
		TickFadeInNextStage(DeltaTime);
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

	case GameFlowState::FadeOutToGameClear:
		TickFadeOutToGameClear(DeltaTime);
		ViewportManager::GetInstance()->Render();
		ViewportManager::GetInstance()->RenderFade();
		DisplayManager::GetInstance()->Render();
		Destroy();
		return;

	case GameFlowState::FadeInGameClear:
		TickFadeInGameClear(DeltaTime);
		GameClear.Render();
		ViewportManager::GetInstance()->RenderFade();
		DisplayManager::GetInstance()->Render();
		return;

	case GameFlowState::GameClear:
		TickGameClear(DeltaTime);
		GameClear.Render();
		DisplayManager::GetInstance()->Render();
		return;

	case GameFlowState::Game:
	default:
		if (ViewportManager::GetInstance()->IsBossIntroPlaying())
		{
			ViewportManager::GetInstance()->Tick(DeltaTime);
			ViewportManager::GetInstance()->Render();
			ViewportManager::GetInstance()->RenderFade();
			DisplayManager::GetInstance()->Render();
			Destroy();
			return;
		}

		StageManager::GetInstance()->Tick(DeltaTime);
		if (StageManager::GetInstance()->ShouldStartGameClear())
		{
			StageManager::GetInstance()->ClearGameClearRequest();
			FlowState = GameFlowState::FadeOutToGameClear;
			ViewportManager::GetInstance()->StartFadeOut();
			ViewportManager::GetInstance()->Render();
			ViewportManager::GetInstance()->RenderFade();
			DisplayManager::GetInstance()->Render();
			Destroy();
			return;
		}

		if (StageManager::GetInstance()->IsPromptOpen())
		{
			TickStagePrompt(DeltaTime);
			ViewportManager::GetInstance()->Render();
			RenderStagePrompt();
			DisplayManager::GetInstance()->Render();
			Destroy();
			return;
		}

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
		if (ViewportManager::GetInstance()->IsBossIntroPlaying())
		{
			ViewportManager::GetInstance()->RenderFade();
		}
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

void GameInstance::TickFadeOutToNextStage(float DeltaTime)
{
	ViewportManager::GetInstance()->TickFade(DeltaTime);
	if (ViewportManager::GetInstance()->IsFadeFinished())
	{
		StageManager::GetInstance()->AdvanceStage();
		ChestManager::GetInstance()->BeginPlay();
		MapManager::GetInstance()->BeginPlay();
		SceneManager::GetInstance()->LoadCurrentStage();
		ViewportManager::GetInstance()->ResetRuntimeCache();
		FlowState = GameFlowState::FadeInNextStage;
		ViewportManager::GetInstance()->StartFadeIn();
	}
}

void GameInstance::TickFadeInNextStage(float DeltaTime)
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

void GameInstance::TickFadeOutToGameClear(float DeltaTime)
{
	ViewportManager::GetInstance()->TickFade(DeltaTime);
	if (ViewportManager::GetInstance()->IsFadeFinished())
	{
		FlowState = GameFlowState::FadeInGameClear;
		ViewportManager::GetInstance()->StartFadeIn();
	}
}

void GameInstance::TickFadeInGameClear(float DeltaTime)
{
	ViewportManager::GetInstance()->TickFade(DeltaTime);
	if (ViewportManager::GetInstance()->IsFadeFinished())
	{
		FlowState = GameFlowState::GameClear;
	}
}

void GameInstance::TickGameClear(float DeltaTime)
{
	GameClearAction action = GameClear.Tick(DeltaTime);
	if (action == GameClearAction::Retry)
	{
		ResetGameWorld();
		FlowState = GameFlowState::FadeInGame;
		ViewportManager::GetInstance()->StartFadeIn();
		return;
	}

	if (action == GameClearAction::MainMenu)
	{
		ResetGameWorld();
		FlowState = GameFlowState::MainMenu;
	}
}

void GameInstance::TickStagePrompt(float DeltaTime)
{
	InputManager* input = InputManager::GetInstance();
	if (input->IsKeyTap(KeyCode::Z))
	{
		if (!StageManager::GetInstance()->IsLastStage())
		{
			StageManager::GetInstance()->CloseStageClearPrompt(false);
			FlowState = GameFlowState::FadeOutToNextStage;
			ViewportManager::GetInstance()->StartFadeOut();
		}
		else
		{
			StageManager::GetInstance()->CloseStageClearPrompt(true);
		}
		return;
	}

	if (input->IsKeyTap(KeyCode::ESCAPE))
	{
		StageManager::GetInstance()->CloseStageClearPrompt(true);
	}
}

void GameInstance::RenderStagePrompt()
{
	DisplayManager* renderer = DisplayManager::GetInstance();
	const int width = 52;
	const int height = 9;
	const int x = max(1, (SCREEN_WIDTH - width) / 2);
	const int y = max(1, (SCREEN_HEIGHT - height) / 2);

	const WORD background = renderer->MakeConsoleAttribute(CC_BLACK, CC_BLACK);
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			renderer->PutCell(y + row, x + col, L' ', background);
		}
	}

	renderer->DrawBox(y, x, width, height);
	const wstring clearText = StageManager::GetInstance()->IsLastStage() ? L"모든 스테이지 클리어!" : L"클리어!";
	const wstring moveText = StageManager::GetInstance()->IsLastStage() ? L"마지막 스테이지입니다." : L"다음 스테이지로 이동하시겠습니까?";
	const wstring inputText = StageManager::GetInstance()->IsLastStage() ? L"(ESC) 닫기" : L"(Z) 예        (ESC) 아니오";

	renderer->AddRender(y + 2, x + (width - renderer->GetTextDisplayWidth(clearText)) / 2, clearText, CC_YELLOW);
	renderer->AddRender(y + 4, x + (width - renderer->GetTextDisplayWidth(moveText)) / 2, moveText, CC_WHITE);
	renderer->AddRender(y + 6, x + (width - renderer->GetTextDisplayWidth(inputText)) / 2, inputText, CC_CYAN);
}

void GameInstance::ResetGameWorld()
{
	ViewportManager::GetInstance()->ResetRuntimeCache();
	ShopManager::GetInstance()->ResetRuntimeCache();
	ShopManager::GetInstance()->RestoreShop();
	CraftingManager::GetInstance()->ResetRuntimeCache();
	EnhancementManager::GetInstance()->ResetRuntimeCache();
	ChestManager::GetInstance()->ResetRuntimeCache();
	BattleManager::GetInstance()->Reset();
	StageManager::GetInstance()->Reset();
	SceneManager::GetInstance()->Reset();
	ViewportManager::GetInstance()->ResetRuntimeCache();
	
	RestartGame();
}

void GameInstance::RestartGame()
{
	ShopManager::GetInstance()->BeginPlay();
	EnhancementManager::GetInstance()->BeginPlay();
	StageManager::GetInstance()->BeginPlay();
	ChestManager::GetInstance()->BeginPlay();
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
