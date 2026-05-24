#pragma once
#include "pch.h"

class UItem;

class Shop
{
private:
	static Shop* Instance;
	Shop() {}
	
	int NumberOfItems = 8;
	vector<const UItem*> Container;


public:
	static Shop* GetInstance()
	{
		if (Instance == nullptr)
			Instance = new Shop();
		return Instance;
	}

	const UItem* GetRandomItem();


};

