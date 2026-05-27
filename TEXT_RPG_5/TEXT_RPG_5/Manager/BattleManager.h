#pragma once
#include "../Singleton.h"
#include "../pch.h"

class BattleManager : public Singleton<BattleManager>
{

public:
	BattleManager();
	~BattleManager() {}

public:
	void Tick(float DeltaTime);
	void BeginPlay();
	void Reset();
	void RegisterKilledMonster(const string& MonsterName);
	const vector<pair<string, int>>& GetKilledMonsters() const { return KilledMonsters; }

private:
	vector<pair<string, int>> KilledMonsters;
};
