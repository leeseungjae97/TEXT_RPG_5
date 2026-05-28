//ChestManager.h

#pragma once

#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/Vector.h"


class UInventoryComponent;
class UItem;

class ChestManager : public Singleton<ChestManager>
{
public:

    ChestManager();
    ~ChestManager();

private:
    int NumberOfItems = 6;
    int MaxColumn = 4;
    int MaxRow = 4;


    unordered_map<int, vector<vector<UItem*>>> Loots;
    vector<Vector> ChestPoses;
    int ActiveKey = -1;

    UInventoryComponent* PlayerInventory = nullptr;
    vector<vector<UItem*>> EmptyContainer;

    static int MakeKey(Vector Pos);
    void FillLoot(vector<vector<UItem*>>& Container);

public:
    vector<Vector>& GetChestPoses() { return ChestPoses; }
    void SetPlayerInventory(UInventoryComponent* Inventory);
    void ResetRuntimeCache();
    void ClearChests(); 
    void RegisterChest(Vector Pos); 
    void BeginPlay();

    int GetMaxColumn() const { return MaxColumn; }
    int GetMaxRow() const { return MaxRow; }

    bool OpenChestAt(Vector Pos); 
    vector<vector<UItem*>>& GetContainerRef();
    UItem* GetItem(Vector Index);
    UItem* GetRandomItem();


    bool StoreItem(UItem* Item);
    void RemoveAt(Vector Index);
};
