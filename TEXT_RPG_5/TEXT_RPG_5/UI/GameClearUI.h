#pragma once

#include "../pch.h"

class DisplayManager;

enum class GameClearAction
{
	None,
	Retry,
	MainMenu,
	RegisterRank
};

class GameClearUI
{
public:
	GameClearAction Tick(float DeltaTime);
	void Render();

private:
	void RenderTitle(int X, int Y);
	void RenderCastle(int X, int Y);
	void RenderMenu(int X, int Y);
	void RenderEpilogue(int X, int Y);

private:
	DisplayManager* Renderer = nullptr;
	int SelectedIndex = 0;
	float StarTimer = 0.0f;
	int StarOffset = 0;
};
