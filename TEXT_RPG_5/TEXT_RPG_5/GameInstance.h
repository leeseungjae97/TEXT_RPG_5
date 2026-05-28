#pragma once
#include <future>

#include "Singleton.h"
#include "UI/MainMenuUI.h"
#include "UI/GameOverUI.h"
#include "UI/GameClearUI.h"
#include "UI/NameInputUI.h"
#include "UI/LeaderboardUI.h"

enum class GameFlowState
{
	MainMenu,
	Leaderboard,
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
	GameClear,
	RankNameConfirm,
	RankNameInput,
	RankSubmitting,
	RankSubmitted
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
	void TickLeaderboard(float DeltaTime);
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
	void TickRankNameConfirm(float DeltaTime);
	void TickRankNameInput(float DeltaTime);
	void TickRankSubmitting(float DeltaTime);
	void TickRankSubmitted(float DeltaTime);
	void TickStagePrompt(float DeltaTime);
	void RenderStagePrompt();
	void RenderStoredTime();
	void RenderRankNameConfirm();
	void RenderRankLoading(const wstring& Message);
	void RenderRankSubmitted();
	void StartRankSubmit(const string& Name);
	void ResetGameWorld();

private:
	bool bQuitRequested = false;
	GameFlowState FlowState = GameFlowState::MainMenu;
	MainMenuUI MainMenu;
	LeaderboardUI Leaderboard;
	NameInputUI NameInput;
	NameInputUI RankNameInput;
	GameOverUI GameOver;
	GameClearUI GameClear;
	future<bool> RankSubmitFuture;
	float RankLoadingTimer = 0.0f;
	int RankLoadingDotCount = 1;
};
