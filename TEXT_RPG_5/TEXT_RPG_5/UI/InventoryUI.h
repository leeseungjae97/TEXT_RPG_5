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
	void DrawItemSlot(int Y, int X, int Width, int Height, const UItem* item);
	void DrawInventoryPanel(int Y, int X, const vector<UItem*>& Items, int Capacity, int Columns, int Rows);
	void DrawEquipmentPanel(int Y, int X, const vector<UItem*>& Items, int Columns, int Rows);
	wchar_t GetItemIcon(const UItem* item);
	
private:
	Player* PlayerPtr;
	UInventoryComponent* InventoryComponent;
	RenderManager* Renderer;
};
