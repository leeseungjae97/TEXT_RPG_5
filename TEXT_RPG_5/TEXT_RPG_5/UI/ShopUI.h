#pragma once
#include "../pch.h"

class Player;
class DisplayManager;
class UInventoryComponent;
class UItem;

class ShopUI
{
public:
	void Render();
	void ResetCache();

private:
	void DrawBackground(int Y, int X, int Width, int Height);
	void DrawItemSlot(int Y, int X, int Width, int Height, const UItem* Item, bool bSelected = false);
	void DrawItemPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title);
	void DrawHoverDialog(int Y, int X, const UItem* Item, bool bSellMode);
	wchar_t GetItemIcon(const UItem* Item);

private:
	Player* PlayerPtr = nullptr;
	UInventoryComponent* InventoryComponent = nullptr;
	DisplayManager* Renderer = nullptr;
};
