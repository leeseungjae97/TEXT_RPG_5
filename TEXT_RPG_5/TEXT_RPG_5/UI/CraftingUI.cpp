#include "CraftingUI.h"

#include "../Component/InventoryComponent.h"
#include "../Define.h"
#include "../Item/Item.h"
#include "../Item/ItemDB.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/CraftingManager.h"
#include "../Manager/EnhancementManager.h"
#include "../Struct/Recipe.h"
#include "../Player.h"
#include "RarityColor.h"

void CraftingUI::ResetCache()
{
	PlayerPtr = nullptr;
	InventoryComponent = nullptr;
}

void CraftingUI::Render()
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

	const bool bEnhancement = InventoryComponent->GetOnEnhancement();
	if (!InventoryComponent->GetOnCrafting() && !bEnhancement)
	{
		return;
	}

	const vector<vector<UItem*>>& items = bEnhancement
		? EnhancementManager::GetInstance()->GetContainerRef()
		: CraftingManager::GetInstance()->GetContainerRef();

	const int slotWidth = 16;
	const int slotHeight = 7;
	const int rows = static_cast<int>(items.size());
	const int columns = rows > 0 ? static_cast<int>(items[0].size()) : 0;
	const int panelWidth = max(1, columns) * (slotWidth - 1) + 1;
	const int panelHeight = 2 + max(1, rows) * (slotHeight - 1) + 1;
	const int x = max(1, (SCREEN_WIDTH - panelWidth) / 2);
	const int y = max(1, (SCREEN_HEIGHT - panelHeight) / 2 - 3);

	DrawBackground(y - 2, x - 3, panelWidth + 6, panelHeight + 6);
	DrawItemPanel(y, x, items, bEnhancement ? L"강화 재료" : L"Crafting");

	UItem* hovered = bEnhancement
		? EnhancementManager::GetInstance()->GetItem(InventoryComponent->GetCursor())
		: CraftingManager::GetInstance()->GetItem(InventoryComponent->GetCursor());
	if (bEnhancement)
	{
		DrawEnhancementInfo(y, x + panelWidth + 4, hovered);
	}
	else
	{
		DrawRecipeInfo(y, x + panelWidth + 4, hovered);
	}

	Renderer->AddRender(y - 1, x, bEnhancement ? L"(Z)강화  (C/A/ESC)닫기" : L"(Z)제작  (A/ESC)닫기");
}

void CraftingUI::DrawBackground(int Y, int X, int Width, int Height)
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

void CraftingUI::DrawItemSlot(int Y, int X, int Width, int Height, const UItem* Item, bool bSelected)
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

	if (itemInfo.Type == ItemType::Equipment)
	{
		wstring rarityName = GetRarityName(itemInfo.Rarity);
		int maxRarityWidth = max(1, Width - 2);
		if (Renderer->GetTextDisplayWidth(rarityName) > maxRarityWidth)
		{
			rarityName = Renderer->TrimTextToDisplayWidth(rarityName, maxRarityWidth);
		}

		int rarityX = X + 1 + max(0, (maxRarityWidth - Renderer->GetTextDisplayWidth(rarityName)) / 2);
		Renderer->AddRender(Y + 1, rarityX, rarityName, GetRarityColor(itemInfo.Rarity));
	}

	wstring itemName = GetDisplayItemName(Item);
	int maxNameWidth = max(1, Width - 2);
	if (Renderer->GetTextDisplayWidth(itemName) > maxNameWidth)
	{
		itemName = Renderer->TrimTextToDisplayWidth(itemName, maxNameWidth);
	}

	int nameX = X + 1 + max(0, (maxNameWidth - Renderer->GetTextDisplayWidth(itemName)) / 2);
	Renderer->AddRender(Y + Height - 2, nameX, itemName, itemInfo.Type == ItemType::Equipment ? GetRarityColor(itemInfo.Rarity) : CC_WHITE);
}

void CraftingUI::DrawItemPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title)
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

void CraftingUI::DrawRecipeInfo(int Y, int X, const UItem* Result)
{
	if (Result == nullptr)
	{
		return;
	}

	const FRecipe* recipe = CraftingManager::GetInstance()->GetRecipeByResult(Result->GetItemInfo().Id);
	if (recipe == nullptr)
	{
		return;
	}

	// 재료 Id별 필요 수량 집계 (중복 = 수량)
	vector<ItemId> distinctIds;
	vector<int> requiredCounts;
	for (ItemId ingredient : recipe->Ingredients)
	{
		bool found = false;
		for (int i = 0; i < static_cast<int>(distinctIds.size()); ++i)
		{
			if (distinctIds[i] == ingredient)
			{
				++requiredCounts[i];
				found = true;
				break;
			}
		}
		if (!found)
		{
			distinctIds.push_back(ingredient);
			requiredCounts.push_back(1);
		}
	}

	const int width = 26;
	const int height = 4 + static_cast<int>(distinctIds.size());
	X = min(max(1, X), max(1, SCREEN_WIDTH - width - 1));
	Y = min(max(1, Y), max(1, SCREEN_HEIGHT - height - 1));

	DrawBackground(Y, X, width, height);
	Renderer->AddRender(Y + 1, X + 2, L"필요 재료");

	for (int i = 0; i < static_cast<int>(distinctIds.size()); ++i)
	{
		int owned = InventoryComponent->CountItemById(distinctIds[i]);
		int required = requiredCounts[i];
		wstring name = Renderer->ToWideString(ItemDB::ItemDBs[static_cast<int>(distinctIds[i])].Name);
		int color = owned >= required ? CC_GREEN : CC_RED;
		Renderer->AddRender(Y + 3 + i, X + 2, name + L" " + to_wstring(owned) + L"/" + to_wstring(required), color);
	}
}

void CraftingUI::DrawEnhancementInfo(int Y, int X, const UItem* Material)
{
	EnhancementManager* Enhancement = EnhancementManager::GetInstance();
	const UItem* Target = Enhancement->GetEnhancementTarget();
	if (Target == nullptr)
	{
		return;
	}

	const int width = 34;
	const int height = 11;
	X = min(max(1, X), max(1, SCREEN_WIDTH - width - 1));
	Y = min(max(1, Y), max(1, SCREEN_HEIGHT - height - 1));

	DrawBackground(Y, X, width, height);

	const FItemInfo& targetInfo = Target->GetItemInfo();
	wstring targetName = Renderer->TrimTextToDisplayWidth(GetDisplayItemName(Target), width - 4);
	Renderer->AddRender(Y + 1, X + 2, L"강화 대상");
	Renderer->AddRender(Y + 2, X + 2, targetName);
	Renderer->AddRender(Y + 3, X + 2, L"강화: +" + to_wstring(Target->GetEnhancementCount()));
	Renderer->AddRender(Y + 4, X + 2, L"Price: " + to_wstring(Target->GetPrice()));

	if (targetInfo.EffectAmount)
	{
		int shownAmount = static_cast<int>(round(targetInfo.EffectAmount * GetRarityStatMultiplier(targetInfo.Rarity)));
		Renderer->AddRender(Y + 5, X + 2, L"Effect: +" + to_wstring(shownAmount));
	}

	if (Material == nullptr)
	{
		Renderer->AddRender(Y + 7, X + 2, L"재료를 선택하세요", CC_DARKGRAY);
		return;
	}

	wstring materialName = Renderer->TrimTextToDisplayWidth(GetDisplayItemName(Material), width - 8);
	const double chance = Enhancement->GetEnhancementChancePercent(Material);
	wstringstream chanceStream;
	if (fabs(chance - round(chance)) < 0.01)
	{
		chanceStream << fixed << setprecision(0) << chance;
	}
	else
	{
		chanceStream << fixed << setprecision(1) << chance;
	}
	Renderer->AddRender(Y + 7, X + 2, L"재료: " + materialName);
	Renderer->AddRender(Y + 8, X + 2, L"성공 확률: " + chanceStream.str() + L"%");

	if (Material->GetEnhancementCount() > Target->GetEnhancementCount())
	{
		Renderer->AddRender(Y + 9, X + 2, L"성공 시 +" + to_wstring(Material->GetEnhancementCount()) + L"로 동기화", CC_GREEN);
	}
	else
	{
		Renderer->AddRender(Y + 9, X + 2, L"성공 시 강화 +1", CC_GREEN);
	}
}

wstring CraftingUI::GetDisplayItemName(const UItem* Item)
{
	if (Item == nullptr)
	{
		return L"";
	}

	const FItemInfo& itemInfo = Item->GetItemInfo();
	wstring itemName = Renderer->ToWideString(itemInfo.Name);
	if (itemInfo.Type == ItemType::Equipment && Item->GetEnhancementCount() > 0)
	{
		itemName += L" +" + to_wstring(Item->GetEnhancementCount());
	}

	return itemName;
}

wchar_t CraftingUI::GetItemIcon(const UItem* Item)
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
