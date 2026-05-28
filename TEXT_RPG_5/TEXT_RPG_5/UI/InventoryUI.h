#pragma once
#include "../pch.h"

class DisplayManager;
class UItem;
class Player;
class UInventoryComponent;

class InventoryUI
{
public:
	InventoryUI();
	~InventoryUI();
	void Render();
	void ResetCache();
	
private:
	void InventoryRender();
	void QuickSlotRender();
	void DrawBackground(int Y, int X, int Width, int Height);
	void DrawItemSlot(int Y, int X, int Width, int Height, const UItem* item, bool bSelected = false);
	void DrawInventoryPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title);
	void DrawEquipmentPanel(int Y, int X);
	void DrawChestPanel(int Y, int X);
	void DrawHoverDialog(int Y, int X, const UItem* item);
	wchar_t GetItemIcon(const UItem* item);
	
private:
	Player* PlayerPtr;
	UInventoryComponent* InventoryComponent;
	DisplayManager* Renderer;
};
