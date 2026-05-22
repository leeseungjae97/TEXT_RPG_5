//Item.h

#pragma once
#include "pch.h"
#include "ItemTypeEnum.h"
#include "Player.h"

struct UItem
{
	string Name;
	ItemType Type;
	int Price;
	int EffectAmount;

	UItem(string name, ItemType type, int price, int effectAmount = 0)
	{
		Name = name;
		Type = type;
		Price = price;
		EffectAmount = effectAmount;
	}
	virtual ~UItem() {}

	void printInfo() const
	{
		
	}

	virtual void Use(Player* player)
	{

	}

};