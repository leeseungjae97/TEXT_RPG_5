//Item.h
#pragma once

#include "../Struct/ItemInfo.h"

class Player;

class UItem
{
protected:
	FItemInfo ItemInfo;
	int EnhancementCount = 0;
	
	
public:
	UItem(const FItemInfo& Info) : ItemInfo(Info){}
	virtual ~UItem() {}
	
	const FItemInfo& GetItemInfo() const { return ItemInfo; }

	ERarity GetRarity() const { return ItemInfo.Rarity; }
	void SetRarity(ERarity Rarity) { ItemInfo.Rarity = Rarity; }

	int GetEnhancementCount() const { return EnhancementCount; }
	void ApplyEnhancementSuccess()
	{
		++EnhancementCount;
		ItemInfo.EffectAmount = static_cast<int>(ceil(ItemInfo.EffectAmount * 1.2f));
		ItemInfo.Price = static_cast<int>(ceil(ItemInfo.Price * 1.2f));
	}
	void MatchEnhancementCount(int TargetCount)
	{
		while (EnhancementCount < TargetCount)
		{
			ApplyEnhancementSuccess();
		}
	}

	// 등급이 반영된 실제 가격 (구매/판매에서 사용)
	int GetPrice() const { return static_cast<int>(round(ItemInfo.Price * GetRarityPriceMultiplier(ItemInfo.Rarity))); }

	void printInfo() const;
	virtual void Use(Player* player);
	virtual void Tick(float DeltaTime);

};
