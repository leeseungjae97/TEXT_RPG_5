#include "LevelComponent.h"
#include "../Player.h"

void LevelComponent::StatsUpdate(int LevelUpCount)
{
    if (Player* player = dynamic_cast<Player*>(GetOwner()))
    {
        if (player->GetLevel() < MaxLevel && player->GetExp() >= player->GetMax_Exp())
        {

            player->SetMax_HP(player->GetLevel() * 20 * LevelUpCount);
            player->SetHP(player->GetMax_HP() * LevelUpCount);
            player->SetPower(player->GetLevel() * 5 * LevelUpCount);
            player->SetLevel(player->GetLevel() + 1 * LevelUpCount);
            player->SetExp(player->GetExp() - player->GetMax_Exp() * LevelUpCount);
        }
    }
}


void LevelComponent::AddExp(int InExp)
{
    if (Player* player = dynamic_cast<Player*>(GetOwner())) {

        player->SetExp(player->GetExp() + InExp);
        int LevelUpCount = player->GetExp() / player->GetMax_Exp();
        if (LevelUpCount >= 1)
        {
            StatsUpdate(LevelUpCount);
        }
    }
}

