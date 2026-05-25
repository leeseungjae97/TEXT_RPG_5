//Strength_Potion.h
#pragma once

#include "Struct/Item.h"

struct Strength_Potion : public UItem
{
	Strength_Potion(string name, ItemType type, int price, int effectAmount = 0);
	~Strength_Potion() {}

	void Use(Player* player) override;
};
