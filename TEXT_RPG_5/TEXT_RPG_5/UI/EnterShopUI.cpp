#include "EnterShopUI.h"

#include "../Player.h"
#include "../Component/InventoryComponent.h"
#include "../Component/MoveComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/StageManager.h"
#include "../Manager/DisplayManager.h"

void EnterShopUI::Close()
{
	bOpen = false;
}

void EnterShopUI::Tick(float DeltaTime)
{
	if (!bOpen)
		return;

	Player* PlayerPtr = SceneManager::GetInstance()->GetPlayer();
	UMoveComponent* MoveComponent = PlayerPtr != nullptr
		? PlayerPtr->GetComponent<UMoveComponent>()
		: nullptr;

	Vector ShopPosition;
	if (MoveComponent == nullptr || !MoveComponent->GetAdjacentShop(ShopPosition))
	{
		Close();
		return;
	}
	
	InputManager* input = InputManager::GetInstance();
	if (input->IsKeyTap(KeyCode::Z))
	{
		if (UInventoryComponent* InventoryComponent = PlayerPtr->GetComponent<UInventoryComponent>())
		{
			InventoryComponent->OpenShop(StageManager::GetInstance()->GetCurrentStage());
			Close();
		}
		
	}
	if (input->IsKeyTap(KeyCode::X) || input->IsKeyTap(KeyCode::ESCAPE))
	{
		MoveComponent->DismissShopPrompt();
		Close();
	}
}

void EnterShopUI::Render()
{
	if (!bOpen)
	{
		return;
	}

	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}
	
	const int width = 44;
	const int height = 9;
	const int x = max(1, (SCREEN_WIDTH - width) / 2);
	const int y = max(1, (SCREEN_HEIGHT - height) / 2);
	WORD background = Renderer->MakeConsoleAttribute(CC_BLACK, CC_BLACK);
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			Renderer->PutCell(y + row, x + col, L' ', background);
		}
	}
	Renderer->DrawBox(y, x, width, height);
	
	Renderer->AddRender(y + 2, x + 10, L"상점 입장");
	Renderer->AddRender(y + 5, x + 7, L"(Z) 입장        (ESC/X) 취소");
}

void EnterShopUI::Open()
{
	bOpen = true;
}
