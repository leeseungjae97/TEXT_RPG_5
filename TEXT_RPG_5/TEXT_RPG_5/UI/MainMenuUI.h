#pragma once

#include "../pch.h"

class DisplayManager;

enum class MainMenuAction
{
	None,
	StartGame,
	Leaderboard,
	Exit
};

class MainMenuUI
{
public:
	MainMenuAction Tick(float DeltaTime);
	void Render();

private:
	void RenderTitle(int X, int Y);
	void RenderLandscape(int X, int Y);
	void RenderMenu(int X, int Y);
	void RenderCredits(int X, int Y);

private:
	DisplayManager* Renderer = nullptr;
	int SelectedIndex = 0;
	bool bShowCredits = false;
	float StarTimer = 0.0f;
	int StarOffset = 0;
};
