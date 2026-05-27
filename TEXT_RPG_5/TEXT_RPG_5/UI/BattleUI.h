#pragma once

#include "../pch.h"

class DisplayManager;

class BattleUI
{
public:
	void Open();
	void Close();
	void Tick(float DeltaTime);
	void Render();
	bool IsOpen() const { return bOpen; }

private:
	bool bOpen = false;
	DisplayManager* Renderer = nullptr;
};
