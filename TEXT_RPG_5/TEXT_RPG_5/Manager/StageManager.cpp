#include "StageManager.h"

#include "DisplayManager.h"
#include "InputManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "../Enum/MapObjectTypeEnum.h"
#include "../Player.h"

void StageManager::BeginPlay()
{
	Reset();
}

void StageManager::Reset()
{
	CurrentStage = 0;
	bBossDefeated = false;
	bHasCrystal = false;
	bPromptOpen = false;
	bPromptDismissedUntilLeave = false;
	bGameClearRequested = false;
	CrystalPosition = { 0, 0 };
	CrystalColorElapsed = 0.0f;
	CrystalColorIndex = 0;
}

void StageManager::Tick(float DeltaTime)
{
	CrystalColorElapsed += DeltaTime;
	if (CrystalColorElapsed >= 0.12f)
	{
		CrystalColorElapsed = 0.0f;
		CrystalColorIndex = (CrystalColorIndex + 1) % 6;
	}

	if (!bBossDefeated || !bHasCrystal || bPromptOpen)
	{
		return;
	}

	Player* player = SceneManager::GetInstance()->GetPlayer();
	if (player == nullptr)
	{
		return;
	}

	Vector playerPosition = player->GetPosition();
	const int distance = abs(playerPosition.X - CrystalPosition.X) + abs(playerPosition.Y - CrystalPosition.Y);
	if (distance <= 1)
	{
		if (!bPromptDismissedUntilLeave)
		{
			OpenStageClearPrompt();
		}
		return;
	}

	bPromptDismissedUntilLeave = false;
}

Vector StageManager::GetPlayerStartPosition() const
{
	switch (CurrentStage)
	{
	case 0: return { 20, 20 };
	case 1: return { 8, 8 };
	case 2: return { 12, 35 };
	default: return { 20, 20 };
	}
}

int StageManager::GetCrystalColor() const
{
	static const int Colors[] = { CC_RED, CC_YELLOW, CC_GREEN, CC_CYAN, CC_BLUE, CC_MAGENTA };
	return Colors[CrystalColorIndex % 6];
}

void StageManager::OnBossKilled(Vector BossPosition)
{
	if (bBossDefeated)
	{
		return;
	}

	bBossDefeated = true;
	if (IsLastStage())
	{
		bGameClearRequested = true;
		return;
	}

	bHasCrystal = true;
	CrystalPosition = BossPosition;
	MapManager::GetInstance()->SetMapObjectCoordinate(CrystalPosition.Y, CrystalPosition.X, { MapObjectType::Crystal, NO_ID });
	OpenStageClearPrompt();
}

void StageManager::OpenStageClearPrompt()
{
	bPromptOpen = true;
}

void StageManager::CloseStageClearPrompt(bool bDismissed)
{
	bPromptOpen = false;
	if (bDismissed)
	{
		bPromptDismissedUntilLeave = true;
	}
}

bool StageManager::AdvanceStage()
{
	if (IsLastStage())
	{
		CloseStageClearPrompt(true);
		return false;
	}

	++CurrentStage;
	bBossDefeated = false;
	bHasCrystal = false;
	bPromptOpen = false;
	bPromptDismissedUntilLeave = false;
	CrystalPosition = { 0, 0 };
	return true;
}
