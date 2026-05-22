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



	void printInfo() const
	{
		std::cout << Name << " (" << Price << "G)\n";
	}

	virtual void Use(Player* player)
	{

		cout << "\n" << Name << " 사용!\n";

	}


};