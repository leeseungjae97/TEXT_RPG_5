#pragma once
#include "LevelComponent.h"
#include "Player.h"

void LevelComponent::StatsUpdate()
{
    if (Player* player = dynamic_cast<Player*>(GetOwner()))
    {
        if (player->GetLevel() <= MaxLevel) {
            player->SetMax_HP(player->GetLevel() * 20);
            player->SetHP(player->GetMax_HP());
            player->SetPower(player->GetLevel() * 5);
            player->SetLevel(player->GetLevel() + 1);
            player->SetExp(player->GetExp() - player->GetMax_Exp());
        }
    }
}