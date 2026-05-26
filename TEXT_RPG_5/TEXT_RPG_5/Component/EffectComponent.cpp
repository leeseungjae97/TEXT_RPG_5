#include "EffectComponent.h"
#include "../Player.h"

UEffectComponent::UEffectComponent(AObject* InOwner)
    : UComponent(InOwner)
{
    PlayerPtr = dynamic_cast<Player*>(InOwner);
}

void UEffectComponent::AddBuff(StatType Stat, int Amount, float Duration)
{
    for (auto& Buff : ActiveBuffs)
    {
        
        if (Buff.Stat == Stat && Buff.Amount == Amount)
        {
            Buff.Elapsed = 0.0f;
            return;
        }
        
        
    }

    ModifyStat(Stat, Amount);
    ActiveBuffs.push_back({ Stat, Amount, Duration, 0.0f });
}

void UEffectComponent::Tick(float DeltaTime)
{
    if (nullptr == PlayerPtr)
    {
        PlayerPtr = dynamic_cast<Player*>(GetOwner());
        
    }

    if (nullptr == PlayerPtr)
        return;

    for (auto& Buff : ActiveBuffs)
        Buff.Elapsed += DeltaTime;

    
    
    ActiveBuffs.erase(
        remove_if(ActiveBuffs.begin(), ActiveBuffs.end(), [&](const FBuff& Buff)
        {
            if (Buff.Elapsed >= Buff.Duration)
            {
                ModifyStat(Buff.Stat, -Buff.Amount);
                return true;
            }
            return false;
        }
        ),
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
