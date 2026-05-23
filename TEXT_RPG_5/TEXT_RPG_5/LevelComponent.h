#pragma once
#include "Component.h";
#include "ComponentTypeEnum.h"
class AObject;

class LevelComponent : public UComponent
{
    friend class AObject;

public:
    static constexpr ComponentType Type = ComponentType::LevelComponent;

    const int MaxLevel = 10;    
    void AddExp(int InExp);
    void StatsUpdate(int LevelUpCount);
    virtual void Tick(float DeltaTime) override {}

private:
    LevelComponent(AObject* InOwner) : UComponent(InOwner) {}
};
