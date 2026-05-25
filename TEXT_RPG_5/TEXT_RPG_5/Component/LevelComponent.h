#pragma once
#include "Component.h";
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

    const int MaxLevel = 10;    
    void AddExp(int InExp);
    void StatsUpdate(int LevelUpCount);
    virtual void Tick(float DeltaTime) override {}

private:
};
