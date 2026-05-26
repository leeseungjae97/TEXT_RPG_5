#pragma once
#include "../pch.h"

class Player;
class RenderManager;
class UInventoryComponent;
class UItem;

class ShopUI
{
public:
	void Render();

private:
	void DrawBackground(int Y, int X, int Width, int Height);
	void DrawItemSlot(int Y, int X, int Width, int Height, const UItem* Item, bool bSelected = false);
	void DrawItemPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title);
	wchar_t GetItemIcon(const UItem* Item);

private:
	Player* PlayerPtr = nullptr;
	UInventoryComponent* InventoryComponent = nullptr;
	RenderManager* Renderer = nullptr;
};
