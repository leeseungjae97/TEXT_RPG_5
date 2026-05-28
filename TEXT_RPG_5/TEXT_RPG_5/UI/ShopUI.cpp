#include "ShopUI.h"

#include "../Component/InventoryComponent.h"
#include "../Define.h"
#include "../Item/Item.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ShopManager.h"
#include "../Player.h"
#include "RarityColor.h"

void ShopUI::ResetCache()
{
	PlayerPtr = nullptr;
	InventoryComponent = nullptr;
}

void ShopUI::Render()
{
	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

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

	if (!InventoryComponent->GetOnShop())
	{
		return;
	}

	ShopManager* shopManager = ShopManager::GetInstance();
	bool bSellMode = shopManager->GetSellMode();
	const vector<vector<UItem*>>& items = bSellMode
		? InventoryComponent->GetContainer()
		: shopManager->GetContainerRef();

	const int slotWidth = 16;
	const int slotHeight = 7;
	const int rows = static_cast<int>(items.size());
	const int columns = rows > 0 ? static_cast<int>(items[0].size()) : 0;
	const int panelWidth = max(1, columns) * (slotWidth - 1) + 1;
	const int panelHeight = 2 + max(1, rows) * (slotHeight - 1) + 1;
	const int x = max(1, (SCREEN_WIDTH - panelWidth) / 2);
	const int y = max(1, (SCREEN_HEIGHT - panelHeight) / 2 - 3);

	DrawBackground(y - 2, x - 3, panelWidth + 6, panelHeight + 6);
	DrawItemPanel(y, x, items, bSellMode ? L"Inventory" : L"Shop");

	Vector cursor = InventoryComponent->GetCursor();
	const UItem* hoveredItem = bSellMode
		? InventoryComponent->GetItem(cursor)
		: shopManager->GetItem(cursor);
	const int hoverY = y + 2 + cursor.Y * (slotHeight - 1);
	const int hoverX = x + cursor.X * (slotWidth - 1) + slotWidth + 2;
	DrawHoverDialog(hoverY, hoverX, hoveredItem, bSellMode);

	Renderer->AddRender(y - 1, x, L"(Z)구매/판매  (C)모드전환  (ESC)닫기");
}

void ShopUI::DrawBackground(int Y, int X, int Width, int Height)
{
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

void ShopUI::DrawItemSlot(int Y, int X, int Width, int Height, const UItem* Item, bool bSelected)
{
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

	if (Item == nullptr)
	{
		return;
	}

	FItemInfo itemInfo = Item->GetItemInfo();
	int iconColor = itemInfo.Type == ItemType::Usable ? CC_GREEN : CC_WHITE;
	Renderer->PutCell(Y + 2, X + Width / 2, GetItemIcon(Item), Renderer->MakeConsoleAttribute(iconColor));

	wstring itemName = Renderer->ToWideString(itemInfo.Name);
	if (itemInfo.Type == ItemType::Equipment && Item->GetEnhancementCount() > 0)
	{
		itemName += L" +" + to_wstring(Item->GetEnhancementCount());
	}
	int maxNameWidth = max(1, Width - 2);
	if (Renderer->GetTextDisplayWidth(itemName) > maxNameWidth)
	{
		itemName = Renderer->TrimTextToDisplayWidth(itemName, maxNameWidth);
	}

	int nameX = X + 1 + max(0, (maxNameWidth - Renderer->GetTextDisplayWidth(itemName)) / 2);
	Renderer->AddRender(Y + Height - 2, nameX, itemName, GetRarityColor(itemInfo.Rarity));
}

void ShopUI::DrawItemPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title)
{
	Vector cursor = InventoryComponent->GetCursor();
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
			bool bSelected = cursor.X == col && cursor.Y == row;
			DrawItemSlot(Y + 2 + row * (slotHeight - 1), X + col * (slotWidth - 1), slotWidth, slotHeight, Items[row][col], bSelected);
		}
	}
}

void ShopUI::DrawHoverDialog(int Y, int X, const UItem* Item, bool bSellMode)
{
	if (Item == nullptr)
	{
		return;
	}

	const int width = 30;
	const int height = 8;
	X = min(max(1, X), max(1, SCREEN_WIDTH - width - 1));
	Y = min(max(1, Y), max(1, SCREEN_HEIGHT - height - 1));

	DrawBackground(Y, X, width, height);

	FItemInfo itemInfo = Item->GetItemInfo();
	wstring itemName = Renderer->ToWideString(itemInfo.Name);
	if (itemInfo.Type == ItemType::Equipment && Item->GetEnhancementCount() > 0)
	{
		itemName += L" +" + to_wstring(Item->GetEnhancementCount());
	}
	itemName = Renderer->TrimTextToDisplayWidth(itemName, width - 4);

	wstring typeText = L"";
	wstring amountContent = L"";
	switch (itemInfo.Type)
	{
	case ItemType::Equipment:
		typeText = L"장비";
		switch (itemInfo.EquipSlot)
		{
		case EquipmentType::Body:
		case EquipmentType::Head:
		case EquipmentType::Boots:
			amountContent = L"MAX HP: +";
			break;
		case EquipmentType::Weapon:
			amountContent = L"POWER: +";
			break;
		default:
			break;
		}
		break;
	case ItemType::Usable:
		typeText = L"소모품";
		amountContent = L"체력 회복: +";
		break;
	case ItemType::Misc:
		typeText = L"재료";
		break;
	default:
		break;
	}

	Renderer->PutCell(Y + 1, X + 2, GetItemIcon(Item), Renderer->MakeConsoleAttribute(CC_YELLOW));
	Renderer->AddRender(Y + 1, X + 4, itemName, GetRarityColor(itemInfo.Rarity));
	if (itemInfo.Type == ItemType::Equipment)
	{
		Renderer->AddRender(Y + 2, X + 2, wstring(L"등급: ") + GetRarityName(itemInfo.Rarity), GetRarityColor(itemInfo.Rarity));
	}
	Renderer->AddRender(Y + 3, X + 2, typeText);

	const int shownPrice = bSellMode ? Item->GetPrice() / 2 : Item->GetPrice();
	Renderer->AddRender(Y + 4, X + 2, wstring(bSellMode ? L"Sell: " : L"Price: ") + to_wstring(shownPrice));
	if (itemInfo.EffectAmount)
	{
		int shownAmount = static_cast<int>(round(itemInfo.EffectAmount * GetRarityStatMultiplier(itemInfo.Rarity)));
		Renderer->AddRender(Y + 5, X + 2, amountContent + to_wstring(shownAmount));
	}
	Renderer->AddRender(Y + 6, X + 2, bSellMode ? L"(Z) 판매" : L"(Z) 구매");
}

wchar_t ShopUI::GetItemIcon(const UItem* Item)
{
	if (Item == nullptr)
	{
		return L' ';
	}

	switch (Item->GetItemInfo().Type)
	{
	case ItemType::Equipment:
		return L'@';
	case ItemType::Usable:
		return L'^';
	case ItemType::Misc:
		return L'*';
	default:
		return L'?';
	}
}
