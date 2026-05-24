//Inventory.h

#pragma once

#include "../Vector.h"
#include "../pch.h"
#include "component.h"
#include "../ComponentTypeEnum.h"

class Player;
class UItem;

class UInventoryComponent : public UComponent
{
	friend class AObject;

public:
	static constexpr ComponentType Type = ComponentType::InventoryComponent;

private:
	Player* PlayerPtr = nullptr;

	float MoveElapsedTime = 0.0f;
	float MoveInterval = 0.12f;
	Vector CurrentCursor = { 0,0 };

protected:
	UInventoryComponent(AObject* InOwner);
	UInventoryComponent() = delete;

	vector<vector<UItem*>> Container;
	int MaxColumn = 4;
	map<int, UItem*> QuickSlot;
	int Gold = 0;
	int SelectedIndex;

public:
	~UInventoryComponent();

	virtual void Tick(float DeltaTime) override;

	void OpenInventory();

	int GetGold(){return Gold;}
	void AddGold(int Amount) {Gold += Amount;} 

	vector<vector<UItem*>>& GetContainer() { return Container; }
	Vector GetItemIndex(UItem* Item);
	UItem* GetItem(Vector Index);
	void AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);
	bool UseRandomItem();
	bool UseItem(UItem* Item);
	
	UItem* GetItemFromCursor(){return Container[CurrentCursor.Y][CurrentCursor.X];}
	bool UseCursorItem(){return UseItem(GetItemFromCursor());}

	map<int, UItem*> GetQuickSlot(){return QuickSlot;}
	void RegisterOnQuickSlot(int Number, UItem* Item){QuickSlot[Number] = Item;}
	UItem* GetItemFromQuickSlot(int Number){return QuickSlot[Number];}
	void UseQuickSlot(int Number){UseItem(QuickSlot[Number]);}

	
	void BuyItem(UItem* Item);
	void SellItem(UItem* Item);

};

