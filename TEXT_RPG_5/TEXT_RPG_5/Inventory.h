//Inventory.h

#pragma once

#include "pch.h"

class Player;
class UItem;

class InventoryManager
{
private:
	Player* Owner = nullptr;

protected:
	vector<UItem*> Container;
	map<int, UItem*> QuickSlot;
	int Gold = 0;
	int SelectedIndex;

public:
	void SetOwner(Player* Owner);
	Player* GetOwner();

	void OpenInventory();

	int GetGold();
	void AddGold(int Amount);

	vector<UItem*>& GetContainer();
	int GetItemIndex(UItem* Item);
	UItem* GetItem(int Index);
	void AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);
	bool UseRandomItem();
	bool UseItem(UItem* Item);

	map<int, UItem*> GetQuickSlot();
	void RegisterOnQuickSlot(int Number, UItem* Item);
	UItem* GetItemFromQuickSlot(int Number);
	void UseQuickSlot(int Number);

	
	void BuyItem(UItem* Item);
	void SellItem(UItem* Item);


};

