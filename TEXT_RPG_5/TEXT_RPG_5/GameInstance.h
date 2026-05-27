#pragma once
#include "Singleton.h"
#include "UI/MainMenuUI.h"
#include "UI/GameOverUI.h"
#include "UI/NameInputUI.h"

enum class GameFlowState
{
	MainMenu,
	NameInput,
	FadeOutToGame,
	Game,
	FadeInGame,
	FadeOutToGameOver,
	FadeInGameOver,
	GameOver
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
	void TickFadeOutToGameOver(float DeltaTime);
	void TickFadeInGameOver(float DeltaTime);
	void TickGameOver(float DeltaTime);
	void ResetGameWorld();

private:
	bool bQuitRequested = false;
	GameFlowState FlowState = GameFlowState::MainMenu;
	MainMenuUI MainMenu;
	NameInputUI NameInput;
	GameOverUI GameOver;
};
