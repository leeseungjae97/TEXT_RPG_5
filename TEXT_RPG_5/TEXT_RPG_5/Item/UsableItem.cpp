// UsableItem.cpp

#include "UsableItem.h"
#include "../Player.h"
#include "../Component/EffectComponent.h"
#include "../Component/CombatComponent.h"
#include "../Component/MoveComponent.h"
#include "../Enum/Direction.h"
#include "ItemDB.h"




void UsableItem::Use(Player* player)
{
    UItem::Use(player);
    if (player == nullptr)
    {
        return;
    }

    UEffectComponent* EffectComponent = player->GetComponent<UEffectComponent>();
    
    switch (ItemInfo.Id)
    {
    case ItemId::HP_POTION:
    case ItemId::DRAGON_HEART:
        player->SetHP(min(player->GetMax_HP(), (player->GetHP() + ItemInfo.EffectAmount)));
        if (EffectComponent != nullptr)
            EffectComponent->PlayItemUseEffect(EItemUseEffectType::Consume);
        break;
        
    case ItemId::STRENGTH_POTION:
        if (EffectComponent != nullptr)
        {
            EffectComponent->AddBuff(StatType::Power, ItemInfo.EffectAmount, 60.0f);
            EffectComponent->PlayItemUseEffect(EItemUseEffectType::Buff);
        }
        break;

    case ItemId::FIRE_WALL_SCROLL:
    {
        Vector Pos = player->GetPosition();
        vector<Vector> CrossRange;
        for (int i = 1; i <= 3; ++i)
        {
            CrossRange.push_back({ Pos.X,     Pos.Y - i });
            CrossRange.push_back({ Pos.X,     Pos.Y + i });
            CrossRange.push_back({ Pos.X - i, Pos.Y     });
            CrossRange.push_back({ Pos.X + i, Pos.Y     });
        }
        player->GetComponent<UCombatComponent>()->TriggerCustomAttack(CrossRange, ItemInfo.EffectAmount);
        if (EffectComponent != nullptr)
            EffectComponent->PlayItemUseEffect(EItemUseEffectType::Consume);
        break;
    }

    case ItemId::FLAME_POTION:
        if (EffectComponent != nullptr)
        {
            EffectComponent->AddPeriodicAttackBuff(ItemInfo.EffectAmount, 1.5f, 10.0f);
            EffectComponent->PlayItemUseEffect(EItemUseEffectType::Buff);
        }
        break;

    case ItemId::LIGHTNING_STRIKE_SCROLL:
    {
        UMoveComponent* Move = player->GetComponent<UMoveComponent>();
        if (Move == nullptr) break;

        EDirection Dir = Move->GetFacingDirection();
        if (Dir == EDirection::NONE) break;

        Vector Pos = player->GetPosition();
        vector<Vector> FanRange;

        for (int dist = 1; dist <= 5; ++dist)
        {
            for (int offset = -(dist - 1); offset <= (dist - 1); ++offset)
            {
                if      (Dir == EDirection::UP)    FanRange.push_back({ Pos.X + offset, Pos.Y - dist });
                else if (Dir == EDirection::DOWN)  FanRange.push_back({ Pos.X + offset, Pos.Y + dist });
                else if (Dir == EDirection::RIGHT) FanRange.push_back({ Pos.X + dist,   Pos.Y + offset });
                else if (Dir == EDirection::LEFT)  FanRange.push_back({ Pos.X - dist,   Pos.Y + offset });
            }
        }

        player->GetComponent<UCombatComponent>()->TriggerCustomAttack(FanRange, ItemInfo.EffectAmount);
        if (EffectComponent != nullptr)
            EffectComponent->PlayItemUseEffect(EItemUseEffectType::Consume);
        break;
    }

    }
    
}
