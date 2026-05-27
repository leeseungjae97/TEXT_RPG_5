#pragma once

#include "../pch.h"

class DisplayManager;

enum class GameOverAction
{
	None,
	Retry,
	MainMenu
};

class GameOverUI
{
public:
	GameOverAction Tick(float DeltaTime);
	void Render();

private:
	void RenderTitle(int X, int Y);
	void RenderGraveyard(int X, int Y);
	void RenderMenu(int X, int Y);

private:
	DisplayManager* Renderer = nullptr;
	int SelectedIndex = 0;
	float StarTimer = 0.0f;
	int StarOffset = 0;
};
