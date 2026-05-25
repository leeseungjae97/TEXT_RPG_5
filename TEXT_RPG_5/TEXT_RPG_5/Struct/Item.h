//Item.h
#pragma once

#include "../pch.h"
#include "../Enum/ItemTypeEnum.h"

class Player;

struct UItem
{
	string Name;
	ItemType Type;
	int Price;
	int EffectAmount;


	UItem(string name, ItemType type, int price, int effectAmount = 0);
	virtual ~UItem() {}

	void printInfo() const;
	virtual void Use(Player* player);
	virtual void Tick(float DeltaTime);

};
