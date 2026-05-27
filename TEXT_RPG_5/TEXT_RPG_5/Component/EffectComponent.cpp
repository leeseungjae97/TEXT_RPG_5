#include "EffectComponent.h"
#include "../Player.h"
#include "CombatComponent.h"

UEffectComponent::UEffectComponent(AObject* InOwner)
    : UComponent(InOwner)
{
    PlayerPtr = dynamic_cast<Player*>(InOwner);
}

void UEffectComponent::AddBuff(StatType Stat, int Amount, float Duration)
{
    for (auto& Buff : ActiveBuffs)
    {
        if (Buff.BuffType == EBuffType::Stat && Buff.Stat == Stat && Buff.Amount == Amount)
        {
            Buff.Elapsed = 0.0f;
            return;
        }
    }

    FBuff NewBuff;
    NewBuff.BuffType = EBuffType::Stat;
    NewBuff.Stat     = Stat;
    NewBuff.Amount   = Amount;
    NewBuff.Duration = Duration;
    ModifyStat(Stat, Amount);
    ActiveBuffs.push_back(NewBuff);
}

void UEffectComponent::AddPeriodicAttackBuff(int Damage, float Period, float Duration)
{
    if (Period <= 0.0f) return;

    FBuff NewBuff;
    NewBuff.BuffType = EBuffType::PeriodicAttack;
    NewBuff.Damage   = Damage;
    NewBuff.Period   = Period;
    NewBuff.Duration = Duration;
    ActiveBuffs.push_back(NewBuff);
}

void UEffectComponent::PlayItemUseEffect(EItemUseEffectType Type, float Duration)
{
    ItemUseEffectType = Type;
    ItemUseEffectDuration = max(0.01f, Duration);
    ItemUseEffectTime = ItemUseEffectDuration;
}

float UEffectComponent::GetItemUseEffectAlpha() const
{
    if (ItemUseEffectDuration <= 0.0f)
    {
        return 1.0f;
    }

    return min(max(1.0f - ItemUseEffectTime / ItemUseEffectDuration, 0.0f), 1.0f);
}

void UEffectComponent::Tick(float DeltaTime)
{
    if (nullptr == PlayerPtr)
    {
        PlayerPtr = dynamic_cast<Player*>(GetOwner());
        
    }

    if (nullptr == PlayerPtr)
        return;

    if (ItemUseEffectTime > 0.0f)
    {
        ItemUseEffectTime -= DeltaTime;
        if (ItemUseEffectTime <= 0.0f)
        {
            ItemUseEffectTime = 0.0f;
            ItemUseEffectType = EItemUseEffectType::None;
        }
    }

    UCombatComponent* Combat = PlayerPtr->GetComponent<UCombatComponent>();

    for (auto& Buff : ActiveBuffs)
    {
        Buff.Elapsed += DeltaTime;

        if (Buff.BuffType == EBuffType::PeriodicAttack && Combat != nullptr)
        {
            Buff.PeriodAcc += DeltaTime;
            if (Buff.PeriodAcc >= Buff.Period)
            {
                Buff.PeriodAcc = 0.0f;
                Vector Pos = PlayerPtr->GetPosition();
                vector<Vector> Ring = {
                    { Pos.X - 1, Pos.Y - 1 }, { Pos.X, Pos.Y - 1 }, { Pos.X + 1, Pos.Y - 1 },
                    { Pos.X - 1, Pos.Y     },                         { Pos.X + 1, Pos.Y     },
                    { Pos.X - 1, Pos.Y + 1 }, { Pos.X, Pos.Y + 1 }, { Pos.X + 1, Pos.Y + 1 },
                };
                Combat->TriggerCustomAttack(Ring, Buff.Damage);
            }
        }
    }

    ActiveBuffs.erase(
        remove_if(ActiveBuffs.begin(), ActiveBuffs.end(), [&](const FBuff& Buff)
        {
            if (Buff.Elapsed >= Buff.Duration)
            {
                if (Buff.BuffType == EBuffType::Stat)
                    ModifyStat(Buff.Stat, -Buff.Amount);
                return true;
            }
            return false;
        }),
        ActiveBuffs.end()
    );
    
    
}




void UEffectComponent::ModifyStat(StatType Stat, int Amount)
{
    switch (Stat)
    {
    case StatType::Power:
        PlayerPtr->SetPower(PlayerPtr->GetPower() + Amount);
        break;
        
        
    }
}
