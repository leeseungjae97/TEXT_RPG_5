#pragma once
#include "../pch.h"

class RenderManager;
class UItem;
class Player;
class UInventoryComponent;

class InventoryUI
{
public:
	InventoryUI();
	~InventoryUI();
	void Render();
	
private:
	void InventoryRender();
	void QuickSlotRender();
	void DrawBackground(int Y, int X, int Width, int Height);
	void DrawItemSlot(int Y, int X, int Width, int Height, const UItem* item, bool bSelected = false);
	void DrawInventoryPanel(int Y, int X, const vector<vector<UItem*>>& Items, const wstring& Title);
	void DrawEquipmentPanel(int Y, int X);
	wchar_t GetItemIcon(const UItem* item);
	
private:
	Player* PlayerPtr;
	UInventoryComponent* InventoryComponent;
	RenderManager* Renderer;
};
