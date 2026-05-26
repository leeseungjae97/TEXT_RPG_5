#pragma once
#include "../Singleton.h"

class BattleManager : public Singleton<BattleManager>
{

public:
	BattleManager();
	~BattleManager() {}

public:
	void Tick(float DeltaTime);
	void BeginPlay();
};
