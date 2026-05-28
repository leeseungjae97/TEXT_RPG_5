#include "ChestOpenUI.h"

#include "../Player.h"
#include "../Component/InventoryComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/StageManager.h"
#include "../Manager/DisplayManager.h"

void ChestOpenUI::Close()
{
	bOpen = false;
}

void ChestOpenUI::Tick(float DeltaTime)
{
	if (!bOpen)
		return;

	Player* PlayerPtr = SceneManager::GetInstance()->GetPlayer();
	UInventoryComponent* InventoryComponent = PlayerPtr != nullptr
		? PlayerPtr->GetComponent<UInventoryComponent>()
		: nullptr;

	if (InventoryComponent == nullptr || !InventoryComponent->IsChestAdjust())
	{
		Close();
		return;
	}
	
	InputManager* input = InputManager::GetInstance();
	if (input->IsKeyTap(KeyCode::Z))
	{
		InventoryComponent->OpenChest();
		Close();
	}
	if (input->IsKeyTap(KeyCode::X) || input->IsKeyTap(KeyCode::ESCAPE))
	{
		InventoryComponent->DismissChestPrompt();
		Close();
	}
}

void ChestOpenUI::Render()
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
	
	Renderer->AddRender(y + 2, x + 10, L"상자 열기");
	Renderer->AddRender(y + 5, x + 7, L"(Z) 확인       (ESC/X) 취소");
}

void ChestOpenUI::Open()
{
	bOpen = true;
}
