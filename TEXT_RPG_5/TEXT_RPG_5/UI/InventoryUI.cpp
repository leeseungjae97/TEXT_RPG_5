#include "InventoryUI.h"

#include "../Component/InventoryComponent.h"
#include "../Manager/RenderManager.h"
#include "../Manager/SceneManager.h"
#include "../Player.h"
#include "../Struct/Item.h"

void InventoryUI::Render()
{
	Player* PlayerPtr = SceneManager::GetInstance()->GetPlayer();
	if (!PlayerPtr)
	{
		return;
	}

	UInventoryComponent* InventoryComponent = PlayerPtr->GetComponent<UInventoryComponent>();
	if (!InventoryComponent)
	{
		return;
	}

	vector<UItem*>& container = InventoryComponent->GetContainer();
	vector<UItem*> inventoryItems;
	vector<UItem*> equipmentItems;

	for (UItem* item : container)
	{
		if (item == nullptr)
		{
			continue;
		}

		if (item->Type == ItemType::Equipment)
		{
			equipmentItems.push_back(item);
		}
		else
		{
			inventoryItems.push_back(item);
		}
	}

	constexpr int inventoryColumns = 7;
	constexpr int inventoryRows = 3;
	constexpr int inventoryCapacity = inventoryColumns * inventoryRows;
	constexpr int equipmentColumns = 2;
	constexpr int equipmentRows = 3;

	const int inventoryX = 3;
	const int inventoryY = 3;
	const int equipmentX = inventoryX + inventoryColumns * 15 + 8;
	const int equipmentY = inventoryY;

	RenderManager* renderManager = RenderManager::GetInstance();
	renderManager->DrawInventoryPanel(inventoryY, inventoryX, inventoryItems, inventoryCapacity, inventoryColumns, inventoryRows);
	renderManager->DrawEquipmentPanel(equipmentY, equipmentX, equipmentItems, equipmentColumns, equipmentRows);
}
