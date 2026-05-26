#include "LevelComponent.h"
#include "../Player.h"

void LevelComponent::StatsUpdate(int LevelUpCount)
{
    if (Player* player = dynamic_cast<Player*>(GetOwner()))
    {
        if (player->GetLevel() < MaxLevel && player->GetExp() >= player->GetMax_Exp())
        {
            int PossibleLevelUpCount = min(LevelUpCount, MaxLevel - player->GetLevel());
            if (PossibleLevelUpCount <= 0)
            {
                return;
            }

            player->SetLevel(player->GetLevel() + LevelIncreaseAmount * PossibleLevelUpCount);
            player->SetMax_HP(player->GetMax_HP() + HPIncreasePerLevel * PossibleLevelUpCount);
            player->SetHP(player->GetMax_HP());
            player->SetPower(player->GetPower() + PowerIncreasePerLevel * PossibleLevelUpCount);
            player->SetExp(player->GetExp() - player->GetMax_Exp() * PossibleLevelUpCount);

            wstring stateText = L"LV " + to_wstring(player->GetLevel())
                + L"  HP " + to_wstring(player->GetHP()) + L"/" + to_wstring(player->GetMax_HP())
                + L"  POW " + to_wstring(player->GetPower());
            NotifyLevelUp(stateText);
        }
    }
}

void LevelComponent::AddExp(int InExp)
{
    if (Player* player = dynamic_cast<Player*>(GetOwner())) {

        player->SetExp(player->GetExp() + InExp);
        LevelUpCount = player->GetExp() / max(1, player->GetMax_Exp());
        if (LevelUpCount >= 1)
        {
            StatsUpdate(LevelUpCount);
        }
    }
}

void LevelComponent::Tick(float DeltaTime)
{
    if (LevelUpDisplayTime > 0.0f)
    {
        LevelUpDisplayTime -= DeltaTime;
        if (LevelUpDisplayTime < 0.0f)
        {
            LevelUpDisplayTime = 0.0f;
        }
    }

    if (Player* player = dynamic_cast<Player*>(GetOwner()))
    {
        LevelUpCount = player->GetExp() / max(1, player->GetMax_Exp());
        if (LevelUpCount >= 1)
        {
            StatsUpdate(LevelUpCount);
        }
    }
}

float LevelComponent::GetLevelUpEffectAlpha() const
{
    if (LevelUpDisplayDuration <= 0.0f)
    {
        return 1.0f;
    }

    return min(max(1.0f - LevelUpDisplayTime / LevelUpDisplayDuration, 0.0f), 1.0f);
}

void LevelComponent::NotifyLevelUp(const wstring& StateText, float DisplayDuration)
{
    LevelUpStateText = StateText;
    LevelUpDisplayTime = DisplayDuration;
    LevelUpDisplayDuration = DisplayDuration;
}
