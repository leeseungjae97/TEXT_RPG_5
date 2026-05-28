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

	ERarity GetRarity() const { return ItemInfo.Rarity; }
	void SetRarity(ERarity Rarity) { ItemInfo.Rarity = Rarity; }

	// 등급이 반영된 실제 가격 (구매/판매에서 사용)
	int GetPrice() const { return static_cast<int>(round(ItemInfo.Price * GetRarityPriceMultiplier(ItemInfo.Rarity))); }

	void printInfo() const;
	virtual void Use(Player* player);
	virtual void Tick(float DeltaTime);

};
