#pragma once
#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
class AObject;

class LevelComponent : public UComponent
{
    friend class AObject;

protected:
    LevelComponent(AObject* InOwner) : UComponent(InOwner) {}
    LevelComponent() = delete;

public:

    static constexpr ComponentType Type = ComponentType::LevelComponent;

    const int MaxLevel = 999;    
    void AddExp(int InExp);
    void StatsUpdate(int LevelUpCount);
    bool ShouldShowLevelUpText() const { return LevelUpDisplayTime > 0.0f; }
    const wstring& GetLevelUpStateText() const { return LevelUpStateText; }
    float GetLevelUpEffectAlpha() const;
    void NotifyLevelUp(const wstring& StateText, float DisplayDuration = 2.0f);
    virtual void Tick(float DeltaTime) override;
    
private:
    int LevelUpCount = 0;
    const int HPIncreasePerLevel = 20;
    const int PowerIncreasePerLevel = 5;
    const int LevelIncreaseAmount = 1;
    float LevelUpDisplayTime = 0.0f;
    float LevelUpDisplayDuration = 0.0f;
    wstring LevelUpStateText;
};
