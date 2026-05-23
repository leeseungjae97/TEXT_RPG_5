#pragma once
#include "pch.h"

class UItem;

class Shop
{
protected:
	int NumberOfItems = 8;
	vector<const UItem*> Container;

public:
	Shop();
	~Shop() {}

	const UItem* GetRandomItem();


};

