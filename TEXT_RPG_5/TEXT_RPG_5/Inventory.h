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
	void AddItem(Item* item);

	bool RemoveItem(Item* item);
	bool UseItem(Item* item);
	


};

