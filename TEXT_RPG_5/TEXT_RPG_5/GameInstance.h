#pragma once
#include "Singleton.h"
#include "UI/MainMenuUI.h"
#include "UI/GameOverUI.h"
#include "UI/GameClearUI.h"
#include "UI/NameInputUI.h"

enum class GameFlowState
{
	MainMenu,
	NameInput,
	FadeOutToGame,
	Game,
	FadeInGame,
	FadeOutToNextStage,
	FadeInNextStage,
	FadeOutToGameOver,
	FadeInGameOver,
	GameOver,
	FadeOutToGameClear,
	FadeInGameClear,
	GameClear
};

class GameInstance : public Singleton<GameInstance>
{

public:
	void Run();

public:
	void Tick();
	void BeginPlay();
	void Render();
	void Destroy();
	void RequestQuit() { bQuitRequested = true; }
	bool IsQuitRequested() const { return bQuitRequested; }
	void RestartGame();

public:
	GameInstance();
	~GameInstance();

private:
	void TickMainMenu(float DeltaTime);
	void TickNameInput(float DeltaTime);
	void TickFadeOutToGame(float DeltaTime);
	void TickFadeInGame(float DeltaTime);
	void TickFadeOutToNextStage(float DeltaTime);
	void TickFadeInNextStage(float DeltaTime);
	void TickFadeOutToGameOver(float DeltaTime);
	void TickFadeInGameOver(float DeltaTime);
	void TickGameOver(float DeltaTime);
	void TickFadeOutToGameClear(float DeltaTime);
	void TickFadeInGameClear(float DeltaTime);
	void TickGameClear(float DeltaTime);
	void TickStagePrompt(float DeltaTime);
	void RenderStagePrompt();
	void ResetGameWorld();

private:
	bool bQuitRequested = false;
	GameFlowState FlowState = GameFlowState::MainMenu;
	MainMenuUI MainMenu;
	NameInputUI NameInput;
	GameOverUI GameOver;
	GameClearUI GameClear;
};
