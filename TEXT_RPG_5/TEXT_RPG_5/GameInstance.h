#pragma once
#include "Singleton.h"

class GameInstance : public Singleton<GameInstance>
{

public:
	void Run();

public:
	void Tick();
	void Init();
	void Render();
	void Destroy();

public:
	GameInstance();
	~GameInstance();
};