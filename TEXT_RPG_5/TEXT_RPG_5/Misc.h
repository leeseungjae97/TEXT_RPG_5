//Misc.h
#pragma once

#include "Item.h"

struct Misc : public Item
{
	Misc(string name, ItemType type, int price, int effectAmount = 0) : Item(name, type, price, effectAmount) {}


};