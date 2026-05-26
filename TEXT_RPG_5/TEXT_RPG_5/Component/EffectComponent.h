#pragma once

#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Enum/StatType.h"
#include "../Struct/Buff.h"
#include "../pch.h"

class Player;

class UEffectComponent : public UComponent
{
    friend class AObject;

public:
    static constexpr ComponentType Type = ComponentType::EffectComponent;

protected:
    UEffectComponent(AObject* InOwner);
    UEffectComponent() = delete;

public:
    void AddBuff(StatType Stat, int Amount, float Duration);
    virtual void Tick(float DeltaTime) override;

private:
    void ModifyStat(StatType Stat, int Amount);

    Player* PlayerPtr = nullptr;
    vector<FBuff> ActiveBuffs;
};
