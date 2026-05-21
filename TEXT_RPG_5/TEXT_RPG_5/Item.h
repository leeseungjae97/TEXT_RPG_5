/*
#pragma once
#include "pch.h"

struct Item
{
	string name;
	int Price;
	int EffectAmount;


	void printInfo() const
	{
		std::cout << name << " (" << price << "G)\n";
	}

	bool useItem(Player* player, Inventory<Item>* container, int index) const
	{
		bool used = false;
		switch (type)
		{
		case ItemType::HP_POTION:
			player->setHP(std::min(player->getHP() + effectAmount, player->getMaxHP()));
			std::cout << "\n" << name << " 사용! HP + " << effectAmount << "\n";
			used = true;
			break;

		case ItemType::MP_POTION:
			player->setMP(std::min(player->getMP() + effectAmount, player->getMaxMP()));
			std::cout << "\n" << name << " 사용! MP + " << effectAmount << "\n";
			used = true;
			break;

		default:
			used = false;
			break;

		}

		if (used == true)
		{
			(*container).RemoveItem(index);
		}
		else
		{
			std::cout << "\n사용할 수 없는 아이템입니다.\n";
		}
		return used;
	}

};

struct Items
{
	Item item;
	int quantity;

};
*/