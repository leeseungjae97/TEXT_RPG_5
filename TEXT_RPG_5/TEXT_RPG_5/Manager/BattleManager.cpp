#include "BattleManager.h"
#include "../Define.h"
#include "../pch.h"
#include "../Object.h"

BattleManager::BattleManager()
{
	
}

void BattleManager::Tick(float DeltaTime)
{
	
}

void BattleManager::BeginPlay()
{
	
}

void BattleManager::Reset()
{
	KilledMonsters.clear();
}

void BattleManager::RegisterKilledMonster(const string& MonsterName)
{
	for (pair<string, int>& killedMonster : KilledMonsters)
	{
		if (killedMonster.first == MonsterName)
		{
			++killedMonster.second;
			return;
		}
	}

	KilledMonsters.push_back({ MonsterName, 1 });
}
