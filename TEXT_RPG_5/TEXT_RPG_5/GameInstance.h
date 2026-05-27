#pragma once
#include "Singleton.h"

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

public:
	GameInstance();
	~GameInstance();

private:
	bool bQuitRequested = false;
};
