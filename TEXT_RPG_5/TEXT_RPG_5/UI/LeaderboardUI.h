#pragma once

#include <future>

#include "../pch.h"

class DisplayManager;

enum class LeaderboardAction
{
	None,
	Back
};

struct LeaderboardEntry
{
	int Rank = 0;
	double TimeSeconds = 0.0;
	int Level = 0;
	wstring Name;
};

class LeaderboardUI
{
public:
	void Open();
	LeaderboardAction Tick(float DeltaTime);
	void Render();

private:
	void StartLoad();
	void FinishLoad(const string& Text);
	vector<LeaderboardEntry> ParseLeaderboardText(const string& Text);
	void RenderTitle(int X, int Y);
	void RenderTable(int X, int Y);
	void RenderTrophy(int X, int Y);
	void RenderLoading();

private:
	DisplayManager* Renderer = nullptr;
	vector<LeaderboardEntry> Entries;
	future<string> LoadFuture;
	bool bIsLoading = false;
	bool bHasLoaded = false;
	float DotTimer = 0.0f;
	int DotCount = 1;
	float StarTimer = 0.0f;
	int StarOffset = 0;
};
