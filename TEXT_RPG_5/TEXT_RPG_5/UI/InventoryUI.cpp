#include "InventoryUI.h"

#include "../Component/InventoryComponent.h"
#include "../Manager/RenderManager.h"
#include "../Manager/SceneManager.h"
#include "../Player.h"
#include "../Item/Item.h"


InventoryUI::InventoryUI() 
	: PlayerPtr(nullptr), InventoryComponent(nullptr), Renderer(nullptr)
{
}

InventoryUI::~InventoryUI()
{
}

void InventoryUI::Render()
{
	InventoryRender();
	QuickSlotRender();
}

void InventoryUI::InventoryRender()
{
	if (!PlayerPtr)
	{
		PlayerPtr = SceneManager::GetInstance()->GetPlayer();
		return;
	}

	if (!InventoryComponent)
	{
		InventoryComponent = PlayerPtr->GetComponent<UInventoryComponent>();
		return;
	}

	vector<vector<UItem*>>& container = InventoryComponent->GetContainer();
	vector<UItem*> equipmentItems;

	for (int i = 0; i < container.size(); i++)
	{
		for (int j = 0; j < container[i].size(); j++)
		{
			UItem* item = container[i][j];
			if (item == nullptr) { continue; }
			FItemInfo ItemInfo = item->GetItemInfo();

			if (ItemInfo.Type == ItemType::Equipment) { equipmentItems.push_back(item); }
		}
	}

	int equipmentColumns = 2;
	int equipmentRows = 3;
	
	const int inventoryX = 3;
	const int inventoryY = 3;
	const int equipmentX = inventoryX + static_cast<int>(container[0].size()) * 15 + 8;
	const int equipmentY = inventoryY;

	DrawInventoryPanel(inventoryY, inventoryX, container);
	DrawEquipmentPanel(equipmentY, equipmentX, equipmentItems, equipmentColumns, equipmentRows);
}

void InventoryUI::QuickSlotRender()
{
	
}

void InventoryUI::DrawItemSlot(int Y, int X, int Width, int Height, const UItem* item, bool bSelected)
{
	if (!Renderer)
	{
		Renderer = RenderManager::GetInstance();
		return;
	}
	Renderer->DrawBox(Y, X, Width, Height);

	if (bSelected)
	{
		WORD selectedAttribute = Renderer->MakeConsoleAttribute(CC_RED);
		for (int dx = 0; dx < Width; ++dx)
		{
			Renderer->PutCell(Y, X + dx, dx == 0 || dx == Width - 1 ? L'+' : L'-', selectedAttribute);
			Renderer->PutCell(Y + Height - 1, X + dx, dx == 0 || dx == Width - 1 ? L'+' : L'-', selectedAttribute);
		}

		for (int dy = 1; dy < Height - 1; ++dy)
		{
			Renderer->PutCell(Y + dy, X, L'|', selectedAttribute);
			Renderer->PutCell(Y + dy, X + Width - 1, L'|', selectedAttribute);
		}
	}
	
	if (item == nullptr)
	{
	    return;
	}
	FItemInfo ItemInfo = item->GetItemInfo();

	WORD iconAttribute = Renderer->MakeConsoleAttribute(CC_CYAN);
	if (ItemInfo.Type == ItemType::Equipment)
	{
	    iconAttribute = Renderer->MakeConsoleAttribute(CC_WHITE);
	}
	else if (ItemInfo.Type == ItemType::Usable)
	{
	    iconAttribute = Renderer->MakeConsoleAttribute(CC_GREEN);
	}
	else if (ItemInfo.Type == ItemType::Misc)
	{
	    iconAttribute = Renderer->MakeConsoleAttribute(CC_YELLOW);
	}
	
	wchar_t icon = GetItemIcon(item);
	Renderer->PutCell(Y + 2, X + Width / 2, icon, iconAttribute);
	
	wstring itemName = Renderer->ToWideString(ItemInfo.Name);
	int maxNameWidth = max(1, Width - 2);
	if (Renderer->GetTextDisplayWidth(itemName) > maxNameWidth)
	{
	    itemName = Renderer->TrimTextToDisplayWidth(itemName, maxNameWidth);
	}
	
	int nameX = X + 1 + max(0, (maxNameWidth - Renderer->GetTextDisplayWidth(itemName)) / 2);
	Renderer->AddRender(Y + Height - 2, nameX, itemName);
}

void InventoryUI::DrawInventoryPanel(int Y, int X, vector<vector<UItem*>>& Items)
{
	if (!Renderer)
	{
		Renderer = RenderManager::GetInstance();
		return;
	}
	
	Vector Cursor = InventoryComponent->GetCursor();
	const int slotWidth = 16;
	const int slotHeight = 7;
	int rows = static_cast<int>(Items.size());
	int columns = rows > 0 ? static_cast<int>(Items[0].size()) : 0;
	int capacity = rows * columns;
	int itemCount = 0;

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < static_cast<int>(Items[row].size()); ++col)
		{
			if (Items[row][col] != nullptr)
			{
				++itemCount;
			}
		}
	}

	Renderer->AddRender(Y, X, L"인벤토리 (" + to_wstring(itemCount) + L"/" + to_wstring(capacity) + L")");

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < static_cast<int>(Items[row].size()); ++col)
		{
			bool bSelected = Cursor.X == col && Cursor.Y == row;
			DrawItemSlot(Y + 2 + row * (slotHeight - 1), X + col * (slotWidth - 1), slotWidth, slotHeight, Items[row][col], bSelected);
		}
	}
}

void InventoryUI::DrawEquipmentPanel(int Y, int X, const vector<UItem*>& Items, int Columns, int Rows)
{
	if (!Renderer)
	{
		Renderer = RenderManager::GetInstance();
		return;
	}
	
	const int slotWidth = 16;
	const int slotHeight = 7;

	Renderer->AddRender(Y, X, L"장비");

	for (int row = 0; row < Rows; ++row)
	{
		for (int col = 0; col < Columns; ++col)
		{
			int index = row * Columns + col;
			const UItem* item = index < Items.size() ? Items[index] : nullptr;
			DrawItemSlot(Y + 2 + row * (slotHeight - 1), X + col * (slotWidth - 1), slotWidth, slotHeight, item);
		}
	}
}

wchar_t InventoryUI::GetItemIcon(const UItem* item)
{
	if (item == nullptr)
	{
		return L' ';
	}

	switch (item->GetItemInfo().Type)
	{
	case ItemType::Equipment:
		return L'@';
	case ItemType::Usable:
		return L'^';
	case ItemType::Misc:
		return L'■';
	default:
		return L'?';
	}
	return L' ';
}
