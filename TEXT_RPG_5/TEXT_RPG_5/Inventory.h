//Inventory.h

#pragma once

#include "pch.h"
#include "Item.h"

class InventoryManager
{
private:
	static InventoryManager* Instance;

	InventoryManager() {}

	vector<UItem*> Container;
	Player* Player;
	

public:
	static InventoryManager* Get()
	{

		if (Instance == nullptr)
			Instance = new InventoryManager();
		return Instance;
	}

	void OpenInventory();

	vector<UItem*>& GetContainer();
	int GetItemIndex(UItem* Item);
	void AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);
	bool UseItem();

	


};

