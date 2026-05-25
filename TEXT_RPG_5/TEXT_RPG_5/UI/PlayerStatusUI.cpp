#include "PlayerStatusUI.h"

#include "../Manager/RenderManager.h"
#include "../Manager/SceneManager.h"
#include "../Player.h"

void PlayerStatusUI::Render()
{
	Player* PlayerPtr = SceneManager::GetInstance()->GetPlayer();
	if (!PlayerPtr)
	{
		return;
	}

	RenderManager* renderManager = RenderManager::GetInstance();
	renderManager->AddRender(1, 16, "LV : " + to_string(PlayerPtr->GetLevel()));
	renderManager->AddRender(2, 16, "HP : " + to_string(PlayerPtr->GetHP()) + "/" + to_string(PlayerPtr->GetMax_HP()));
	renderManager->AddRender(3, 16, "EXP : " + to_string(PlayerPtr->GetExp()) + "/" + to_string(PlayerPtr->GetMax_Exp()));
	renderManager->AddRender(4, 16, "GOLD : " + to_string(PlayerPtr->GetGold()));
	renderManager->AddRender(5, 16, "POWER : " + to_string(PlayerPtr->GetPower()));
}
