#pragma once

#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Enum/StatType.h"
#include "../Struct/Buff.h"
#include "../pch.h"

class Player;

enum class EItemUseEffectType
{
    None,
    Consume,
    Buff
};

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
    void AddPeriodicAttackBuff(int Damage, float Period, float Duration);
    void PlayItemUseEffect(EItemUseEffectType Type, float Duration = 0.8f);
    bool ShouldShowItemUseEffect() const { return ItemUseEffectTime > 0.0f && ItemUseEffectType != EItemUseEffectType::None; }
    EItemUseEffectType GetItemUseEffectType() const { return ItemUseEffectType; }
    float GetItemUseEffectAlpha() const;
    virtual void Tick(float DeltaTime) override;

private:
    void ModifyStat(StatType Stat, int Amount);

    Player* PlayerPtr = nullptr;
    vector<FBuff> ActiveBuffs;
    EItemUseEffectType ItemUseEffectType = EItemUseEffectType::None;
    float ItemUseEffectTime = 0.0f;
    float ItemUseEffectDuration = 0.8f;
};
