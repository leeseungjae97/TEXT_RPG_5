#pragma once

#include "../Singleton.h"
#include "../Struct/Vector.h"
#include "../Define.h"

class StageManager : public Singleton<StageManager>
{
public:
	void BeginPlay();
	void Reset();
	void Tick(float DeltaTime);

	int GetCurrentStage() const { return CurrentStage; }
	int GetStageCount() const { return StageCount; }
	Vector GetCrystalPosition() const { return CrystalPosition; }
	bool IsBossDefeated() const { return bBossDefeated; }
	bool HasCrystal() const { return bHasCrystal; }
	bool IsPromptOpen() const { return bPromptOpen; }
	bool IsLastStage() const { return CurrentStage >= StageCount - 1; }
	bool ShouldStartGameClear() const { return bGameClearRequested; }
	void ClearGameClearRequest() { bGameClearRequested = false; }
	int GetCrystalColor() const;

	void OnBossKilled(Vector BossPosition);
	void OpenStageClearPrompt();
	void CloseStageClearPrompt(bool bDismissed);
	bool AdvanceStage();

private:
	static constexpr int StageCount = 4;
	int CurrentStage = 0;
	bool bBossDefeated = false;
	bool bHasCrystal = false;
	bool bPromptOpen = false;
	bool bPromptDismissedUntilLeave = false;
	bool bGameClearRequested = false;
	Vector CrystalPosition = { 0, 0 };
	float CrystalColorElapsed = 0.0f;
	int CrystalColorIndex = 0;
};
