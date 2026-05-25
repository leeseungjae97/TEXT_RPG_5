//Inventory.h

#pragma once

#include "../Struct/Vector.h"
#include "../pch.h"
#include "component.h"
#include "../Enum/ComponentTypeEnum.h"

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
	UItem* InventorySlot[4][4] = { nullptr };
	Vector CurrentCursor = { 0,0 };

protected:
	UInventoryComponent(AObject* InOwner);
	UInventoryComponent() = delete;

	vector<UItem*> Container; 
	map<int, UItem*> QuickSlot;
	int Gold = 0;
	int SelectedIndex;

public:
	~UInventoryComponent();

	virtual void Tick(float DeltaTime) override;

	void OpenInventory();

	int GetGold();
	void AddGold(int Amount);

	vector<UItem*>& GetContainer() { return Container; }
	int GetItemIndex(UItem* Item);
	UItem* GetItem(int Index);
	void AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);
	bool UseRandomItem();
	bool UseItem(UItem* Item);
	
	void UpdateInventorySlot();
	UItem* GetItemFromCursor();
	bool UseCursorItem();

	map<int, UItem*> GetQuickSlot();
	void RegisterOnQuickSlot(int Number, UItem* Item);
	UItem* GetItemFromQuickSlot(int Number);
	void UseQuickSlot(int Number);

	
	void BuyItem(UItem* Item);
	void SellItem(UItem* Item);

};

