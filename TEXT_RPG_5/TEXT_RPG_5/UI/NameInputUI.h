#pragma once

#include "../pch.h"

class DisplayManager;

enum class NameInputAction
{
	None,
	Start,
	Cancel
};

class NameInputUI
{
public:
	void Open();
	NameInputAction Tick(float DeltaTime);
	void Render();
	const wstring& GetPlayerName() const { return PlayerName; }

private:
	DisplayManager* Renderer = nullptr;
	wstring PlayerName;
	float CursorTimer = 0.0f;
	bool bShowCursor = true;
	int MaxNameWidth = 20;
};
