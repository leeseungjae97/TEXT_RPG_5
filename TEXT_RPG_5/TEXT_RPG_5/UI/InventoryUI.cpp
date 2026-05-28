#include "InventoryUI.h"

#include "../Component/EquipmentComponent.h"
#include "../Component/InventoryComponent.h"
#include "../Define.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ChestManager.h"
#include "../Player.h"
#include "../Item/Item.h"
#include "RarityColor.h"


InventoryUI::InventoryUI()
	: PlayerPtr(nullptr), InventoryComponent(nullptr), Renderer(nullptr)
{
}

InventoryUI::~InventoryUI()
{
}

void InventoryUI::ResetCache()
{
	PlayerPtr = nullptr;
	InventoryComponent = nullptr;
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
	const bool bChest = InventoryComponent->GetOnChest();
	const int slotWidth = 16;
	const int slotHeight = 7;
	const int slotStepX = slotWidth - 1;
	const int slotStepY = slotHeight - 1;
	const int inventoryColumns = static_cast<int>(container[0].size());
	const int inventoryPanelWidth = inventoryColumns * slotStepX + 1;
	const int equipmentX = inventoryX + inventoryPanelWidth + 8;
	const int equipmentY = inventoryY;
	int backgroundWidth = SCREEN_WIDTH / 2;
	if (bChest)
	{
		const vector<vector<UItem*>>& chestItems = ChestManager::GetInstance()->GetContainerRef();
		const int chestRows = static_cast<int>(chestItems.size());
		const int chestColumns = chestRows > 0 ? static_cast<int>(chestItems[0].size()) : 0;
		const int chestPanelWidth = max(1, chestColumns) * slotStepX + 1;
		backgroundWidth = min(SCREEN_WIDTH - 2, max(backgroundWidth, equipmentX + chestPanelWidth + 3));
	}

	DrawBackground(1, 1, backgroundWidth, SCREEN_HEIGHT / 2);
	Renderer->AddRender(2, 3, bChest ? L"(Z)교환  (TAB/ESC)닫기" : L"(Z)적용/착용  (A)조합  (C)강화  (I/TAB/ESC)닫기");
	DrawInventoryPanel(inventoryY, inventoryX, container, L"인벤토리", slotWidth, slotHeight);
	if (bChest)
		DrawChestPanel(equipmentY, equipmentX, slotWidth, slotHeight);
	else
		DrawEquipmentPanel(equipmentY, equipmentX);

	Vector cursor = InventoryComponent->GetCursor();
	const UItem* hoveredItem = nullptr;
	int hoverY = inventoryY + 2;
	int hoverX = equipmentX + 18;
	if (bChest && InventoryComponent->GetOnChestPanel())
	{
		hoveredItem = ChestManager::GetInstance()->GetItem(cursor);
		hoverY = equipmentY + 2 + cursor.Y * slotStepY;
		hoverX = equipmentX + cursor.X * slotStepX + slotWidth + 2;
	}
	else if (!bChest && InventoryComponent->GetOnEquipment())
	{
		if (UEquipmentComponent* equipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>())
		{
			hoveredItem = equipmentComponent->GetItem(cursor);
		}
		hoverY = equipmentY + 2 + cursor.Y * 6;
		hoverX = equipmentX + 18;
	}
	else
	{
		hoveredItem = InventoryComponent->GetItem(cursor);
		hoverY = inventoryY + 2 + cursor.Y * slotStepY;
		hoverX = inventoryX + cursor.X * slotStepX + slotWidth + 2;
	}
	DrawHoverDialog(hoverY, hoverX, hoveredItem);
}

void InventoryUI::QuickSlotRender()
{
	
}

void InventoryUI::DrawBackground(int Y, int X, int Width, int Height)
{
	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
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
		Renderer = DisplayManager::GetInstance();
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
	if (ItemInfo.Type == ItemType::Equipment && item->GetEnhancementCount() > 0)
	{
	    itemName += L" +" + to_wstring(item->GetEnhancementCount());
	}
	int maxNameWidth = max(1, Width - 2);
	if (Renderer->GetTextDisplayWidth(itemName) > maxNameWidth)
	{
	    itemName = Renderer->TrimTextToDisplayWidth(itemName, maxNameWidth);
	}
	
	int nameX = X + 1 + max(0, (maxNameWidth - Renderer->GetTextDisplayWidth(itemName)) / 2);
	Renderer->AddRender(Y + Height - 2, nameX, itemName, GetRarityColor(ItemInfo.Rarity));
}

void InventoryUI::DrawInventoryPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title, int SlotWidth, int SlotHeight)
{
	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}
	
	Vector Cursor = InventoryComponent->GetCursor();
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
			bool bSelected = !InventoryComponent->GetOnEquipment() && !InventoryComponent->GetOnChestPanel() && Cursor.X == col && Cursor.Y == row;
			DrawItemSlot(Y + 2 + row * (SlotHeight - 1), X + col * (SlotWidth - 1), SlotWidth, SlotHeight, Items[row][col], bSelected);
		}
	}
}

void InventoryUI::DrawEquipmentPanel(int Y, int X)
{
	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
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

void InventoryUI::DrawChestPanel(int Y, int X, int SlotWidth, int SlotHeight)
{
	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

	const vector<vector<UItem*>>& items = ChestManager::GetInstance()->GetContainerRef();
	Vector cursor = InventoryComponent->GetCursor();
	const int rows = static_cast<int>(items.size());
	const int columns = rows > 0 ? static_cast<int>(items[0].size()) : 0;
	int capacity = rows * columns;
	int itemCount = 0;

	for (int row = 0; row < rows; ++row)
		for (int col = 0; col < static_cast<int>(items[row].size()); ++col)
			if (items[row][col] != nullptr)
				++itemCount;

	Renderer->AddRender(Y, X, L"상자 (" + to_wstring(itemCount) + L"/" + to_wstring(capacity) + L")");

	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < static_cast<int>(items[row].size()); ++col)
		{
			bool bSelected = InventoryComponent->GetOnChestPanel() && cursor.X == col && cursor.Y == row;
			DrawItemSlot(Y + 2 + row * (SlotHeight - 1), X + col * (SlotWidth - 1), SlotWidth, SlotHeight, items[row][col], bSelected);
		}
	}
}

void InventoryUI::DrawHoverDialog(int Y, int X, const UItem* item)
{
	if (item == nullptr)
	{
		return;
	}

	const int width = 28;
	const int height = 8;
	X = min(max(1, X), max(1, SCREEN_WIDTH - width - 1));
	Y = min(max(1, Y), max(1, SCREEN_HEIGHT - height - 1));

	DrawBackground(Y, X, width, height);

	FItemInfo itemInfo = item->GetItemInfo();
	wstring itemName = Renderer->ToWideString(itemInfo.Name);
	if (itemInfo.Type == ItemType::Equipment && item->GetEnhancementCount() > 0)
	{
		itemName += L" +" + to_wstring(item->GetEnhancementCount());
	}
	itemName = Renderer->TrimTextToDisplayWidth(itemName, width - 4);
	wstring typeText = L"";
	wstring amountContent = L"";
	
	switch (itemInfo.Type)
	{
	case ItemType::Equipment:
		{
			typeText += L"장비";
			switch (itemInfo.EquipSlot)
			{
			case EquipmentType::Body:
			case EquipmentType::Head:
			case EquipmentType::Boots:
				{
					amountContent += L"MAX HP: +";		
				}
				break;
			case EquipmentType::Weapon:
				{
					amountContent += L"POWER: +";		
				}
				break;
			default:
				break;
			}
			
		}
		break;
	case ItemType::Usable:
		typeText += L"소모품";
		amountContent += L"체력 회복: +";
		break;
	case ItemType::Misc:
		typeText += L"재료";
		break;
	default:
		typeText += L"";
		break;
	}

	Renderer->PutCell(Y + 1, X + 2, GetItemIcon(item), Renderer->MakeConsoleAttribute(CC_YELLOW));
	Renderer->AddRender(Y + 1, X + 4, itemName, GetRarityColor(itemInfo.Rarity));
	if (itemInfo.Type == ItemType::Equipment)
		Renderer->AddRender(Y + 2, X + 2, wstring(L"등급: ") + GetRarityName(itemInfo.Rarity), GetRarityColor(itemInfo.Rarity));
	Renderer->AddRender(Y + 3, X + 2, typeText);
	Renderer->AddRender(Y + 4, X + 2, L"Price: " + to_wstring(item->GetPrice()));
	if (itemInfo.EffectAmount)
	{
		int shownAmount = static_cast<int>(round(itemInfo.EffectAmount * GetRarityStatMultiplier(itemInfo.Rarity)));
		Renderer->AddRender(Y + 5, X + 2, amountContent + to_wstring(shownAmount));
	}
	Renderer->AddRender(Y + 6, X + 2, itemInfo.Type == ItemType::Equipment ? L"(A) 조합  (C) 강화" : L"(A) 조합하기");
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
