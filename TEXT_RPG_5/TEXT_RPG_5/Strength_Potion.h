#pragma once
//Strength_Potion.h
#pragma once

#include "Item.h"

struct Strength_Potion : public UItem
{
	Strength_Potion(string name, ItemType type, int price, int effectAmount = 0) : Item(name, type, price, effectAmount) {}
	~Strength_Potion() {}

	void Use(Player* player) override
	{
		UItem::Use(player);

	}
};