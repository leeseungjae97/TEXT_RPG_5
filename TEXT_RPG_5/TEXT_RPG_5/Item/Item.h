//Item.h
#pragma once

#include "../Struct/ItemInfo.h"

class Player;

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
