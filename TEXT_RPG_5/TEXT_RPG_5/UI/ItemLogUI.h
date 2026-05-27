#pragma once

#include "../pch.h"

class DisplayManager;
class UItem;

struct FItemLogEntry
{
	wstring Text;
	float RemainTime = 0.0f;
};

class ItemLogUI
{
public:
	void AddItemLog(const UItem* Item);
	void AddLog(const wstring& Text);
	void Tick(float DeltaTime);
	void Render();
	void Reset();

private:
	static constexpr int MaxLogCount = 7;
	static constexpr float LogDuration = 1.0f;

	vector<FItemLogEntry> Logs;
	DisplayManager* Renderer = nullptr;
};
