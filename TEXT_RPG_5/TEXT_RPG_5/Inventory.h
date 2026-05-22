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
	

public:
	void SetOwner(Player* Owner);
	Player* GetOwner();

	void OpenInventory();

	vector<UItem*>& GetContainer();
	int GetItemIndex(UItem* Item);
	void AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);
	bool UseItem();

	


};

