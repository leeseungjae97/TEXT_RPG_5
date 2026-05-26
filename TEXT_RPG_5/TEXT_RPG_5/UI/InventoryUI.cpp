#include "InventoryUI.h"

#include "../Component/EquipmentComponent.h"
#include "../Component/InventoryComponent.h"
#include "../Define.h"
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

	if (InventoryComponent->GetOnShop())
	{
		return;
	}

	vector<vector<UItem*>>& container = InventoryComponent->GetContainer();
	
	const int inventoryX = 3;
	const int inventoryY = 3;
	const int equipmentX = inventoryX + static_cast<int>(container[0].size()) * 15 + 8;
	const int equipmentY = inventoryY;

	DrawBackground(1, 1, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	Renderer->AddRender(2, 3, L"(Z)적용/착용  (I)닫기");
	DrawInventoryPanel(inventoryY, inventoryX, container, L"인벤토리");
	DrawEquipmentPanel(equipmentY, equipmentX);
}

void InventoryUI::QuickSlotRender()
{
	
}

void InventoryUI::DrawBackground(int Y, int X, int Width, int Height)
{
	if (!Renderer)
	{
		Renderer = RenderManager::GetInstance();
		return;
	}

	WORD attribute = Renderer->MakeConsoleAttribute(CC_BLACK, CC_BLACK);
	for (int row = 0; row < Height; ++row)
	{
		for (int col = 0; col < Width; ++col)
		{
			Renderer->PutCell(Y + row, X + col, L' ', attribute);
		}
	}

	Renderer->DrawBox(Y, X, Width, Height);
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

void InventoryUI::DrawInventoryPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title)
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

	Renderer->AddRender(Y, X, Title + L" (" + to_wstring(itemCount) + L"/" + to_wstring(capacity) + L")");

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < static_cast<int>(Items[row].size()); ++col)
		{
			bool bSelected = !InventoryComponent->GetOnEquipment() && Cursor.X == col && Cursor.Y == row;
			DrawItemSlot(Y + 2 + row * (slotHeight - 1), X + col * (slotWidth - 1), slotWidth, slotHeight, Items[row][col], bSelected);
		}
	}
}

void InventoryUI::DrawEquipmentPanel(int Y, int X)
{
	if (!Renderer)
	{
		Renderer = RenderManager::GetInstance();
		return;
	}
	
	const int slotWidth = 16;
	const int slotHeight = 7;
	const int Rows = 4;
	const wchar_t* defaultLabels[Rows] = { L"무기", L"머리", L"가슴", L"신발" };

	Renderer->AddRender(Y, X, L"장비");

	for (int row = 0; row < Rows; ++row)
	{
		const UItem* item = nullptr;
		if (PlayerPtr != nullptr)
		{
			UEquipmentComponent* equipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>();
			if (equipmentComponent != nullptr)
			{
				item = equipmentComponent->GetItem({ 0, row });
			}
		}

		bool bSelected = InventoryComponent != nullptr &&
			InventoryComponent->GetOnEquipment() &&
			InventoryComponent->GetCursor().X == 0 &&
			InventoryComponent->GetCursor().Y == row;

		int slotY = Y + 2 + row * (slotHeight - 1);
		DrawItemSlot(slotY, X, slotWidth, slotHeight, item, bSelected);

		if (item == nullptr)
		{
			wstring label = defaultLabels[row];
			int labelX = X + max(1, (slotWidth - Renderer->GetTextDisplayWidth(label)) / 2);
			Renderer->AddRender(slotY + slotHeight / 2, labelX, label);
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
