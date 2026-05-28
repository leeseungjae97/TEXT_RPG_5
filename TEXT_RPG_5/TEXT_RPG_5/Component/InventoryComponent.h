//Inventory.h

#pragma once

#include "../Struct/Vector.h"
#include "../pch.h"
#include "component.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Enum/ItemIdEnum.h"

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
	int MaxRow = 4;
	vector<UItem*> QuickSlot;
	int Gold = 0;
	int SelectedIndex;
	bool bOnShop = false;
	bool bOnEquipment = false;
	bool bOpenedInventory = false;
	bool bOnCrafting = false;
	bool bOnChest = false;       // 상자 열림 (인벤토리와 나란히 표시)
	bool bChestPanel = false;    // 커서가 상자 그리드에 있는지 (false면 인벤토리 그리드)
	


public:
	~UInventoryComponent();

	virtual void Tick(float DeltaTime) override;

	void OpenInventory();
	void OpenShop(int ShopId);
	void CloseInventory();
	bool GetOpenedInventory(){return bOpenedInventory;}

	int GetGold(){return Gold;}
	void SetGold(int Value) { Gold = Value; }
	void AddGold(int Amount) {Gold += Amount;}

	vector<vector<UItem*>>& GetContainer() { return Container; }
	Vector GetItemIndex(UItem* Item);
	UItem* GetItem(Vector Index);
	bool IsFull();
	bool AddItem(UItem* Item);
	bool AcquireItem(UItem* Item);   // 소유권을 넘김. 가득 차면 Item을 delete하고 false (AddItem과 계약이 다름)
	bool RemoveItem(UItem* Item);
	bool UseRandomItem();
	bool UseItem(UItem* Item, bool bShowDialog = false);
	UItem* FindItemById(ItemId Id);
	int CountItemById(ItemId Id);
	
	void SelectCursor();
	void ExpandRow(int Amount){MaxRow += Amount;}
	Vector GetCursor() {return CurrentCursor;}
	void ResetCursor() {CurrentCursor = Vector(0,0);}
	Vector CursorUp();
	Vector CursorDown();
	Vector CursorLeft();
	Vector CursorRight();
	UItem* GetItemFromCursor(){return Container[CurrentCursor.Y][CurrentCursor.X];}
	bool UseCursorItem(){return UseItem(GetItemFromCursor());}

	
	vector<UItem*> GetQuickSlot(){return QuickSlot;}
	void RegisterOnQuickSlot(int Number);
	UItem* GetItemFromQuickSlot(int Number){return QuickSlot[Number];}
	void UseQuickSlot(int Number);
	void ClearQuickSlot(UItem* Item);
	
	bool SetOnShop(bool OnShop);
	bool ToggleOnShop();
	bool GetOnShop(){return bOnShop;}
	bool BuyItem(UItem* Item);
	void SellItem(UItem* Item);

	void ToggleCrafting();
	bool GetOnCrafting(){return bOnCrafting;}

	bool OpenChest();   // 인접한 상자를 열면 true, 없으면 false
	bool GetOnChest(){return bOnChest;}
	bool GetOnChestPanel(){return bChestPanel;}
	bool GetAdjacentChest(Vector& OutPos);

	vector<vector<UItem*>>& GetFocusedContainer();

	bool GetOnEquipment() { return bOnEquipment; }
	void OpenEquipmentPanel() { bOnEquipment = true; ResetCursor(); }
	void CloseEquipmentPanel() { bOnEquipment = false; ResetCursor(); }
	UItem* DetachItem(UItem* Item);
	
	
	
	
	

};

