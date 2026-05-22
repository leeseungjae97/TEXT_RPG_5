//Item.h

#pragma once
#include "pch.h"
#include "ItemTypeEnum.h"
#include "Player.h"

struct Item
{
	string Name;
	ItemType Type;
	int Price;
	int EffectAmount;

	Item(string name, ItemType type, int price, int effectAmount = 0)
	{
		Name = name;
		Type = type;
		Price = price;
		EffectAmount = effectAmount;
	}


	void printInfo() const
	{
		std::cout << Name << " (" << Price << "G)\n";
	}

	virtual void Use(Player* player)
	{

		cout << "\n" << Name << " 사용!\n";

	}


};