//Inventory.h

#pragma once

#include "pch.h"
#include "Item.h"

class Inventory
{
protected:
	vector<Item*> Container;
	Player* Player;


public:
	void printInventory();

	vector<Item*> GetContainer();
	int GetItemIndex(Item* item);
	void AddItem(Item* item);
	bool RemoveItem(Item* item);
	bool UseItem();

	


};

