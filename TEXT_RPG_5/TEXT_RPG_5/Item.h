//Item.h

#pragma once
#include "pch.h"
#include "ItemTypeEnum.h"
#include "ItemIdEnum.h"

class Player;

struct FItemInfo
{
	string Name;
	ItemType Type;
	ItemId Id;
	int Price;
	int EffectAmount;
	
};

class UItem
{
protected:
	FItemInfo ItemInfo;
	
	
	
public:
	UItem(const FItemInfo& Info) : ItemInfo(Info){}
	virtual ~UItem() {}
	
	const FItemInfo& GetItemInfo() const { return ItemInfo; }
	
	void printInfo() const;
	virtual void Use(Player* player);
	virtual void Tick(float DeltaTime);

};
