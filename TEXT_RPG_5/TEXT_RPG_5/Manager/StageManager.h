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
	void SetCurrentStage(int InCurrentStage) { CurrentStage = InCurrentStage;}
	int GetMaxStageCount() const { return MaxStageCount; }
	Vector GetCrystalPosition() const { return CrystalPosition; }
	bool IsBossDefeated() const { return bBossDefeated; }
	bool HasCrystal() const { return bHasCrystal; }
	bool IsPromptOpen() const { return bPromptOpen; }
	bool IsLastStage() const { return CurrentStage >= MaxStageCount; }
	bool ShouldStartGameClear() const { return bGameClearRequested; }
	void ClearGameClearRequest() { bGameClearRequested = false; }
	bool HasBossIntroPlayed() const { return bBossIntroPlayed; }
	bool CanStartBossIntro() const { return !bBossIntroPlayed && !bBossDefeated; }
	void MarkBossIntroPlayed() { bBossIntroPlayed = true; }
	int GetCrystalColor() const;

	void OnBossKilled(Vector BossPosition);
	void OpenStageClearPrompt();
	void CloseStageClearPrompt(bool bDismissed);
	bool AdvanceStage();

private:
	static constexpr int MaxStageCount = 3;
	int CurrentStage = 0;
	bool bBossDefeated = false;
	bool bHasCrystal = false;
	bool bPromptOpen = false;
	bool bPromptDismissedUntilLeave = false;
	bool bGameClearRequested = false;
	bool bBossIntroPlayed = false;
	Vector CrystalPosition = { 0, 0 };
	float CrystalColorElapsed = 0.0f;
	int CrystalColorIndex = 0;
};
