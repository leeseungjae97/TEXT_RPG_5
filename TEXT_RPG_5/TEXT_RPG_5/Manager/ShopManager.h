//ShopManager.h

#pragma once

#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/Vector.h"


class UInventoryComponent;
class UItem;

class ShopManager : public Singleton<ShopManager>
{
public:
    
    ShopManager();
    ~ShopManager();
    
private:
    int NumberOfItems = 8;
    int MaxColumn = 4;
    int MaxRow = 4;
    vector<vector<UItem*>> Container;
    UInventoryComponent* PlayerInventory = nullptr;
    Vector CurrentCursor = {0,0};
    
    bool bSellMode = false;
    int LastVisitedShopId = -1;

    vector<Vector> ShopPoses;

    
public:
    vector<Vector>& GetShopPoses() { return ShopPoses; }
    void SetPlayerInventory(UInventoryComponent* Inventory);
    void ResetRuntimeCache();
    void RestoreShop();
    void BeginPlay();

    vector<vector<UItem*>> GetContainer() {return Container;}
    vector<vector<UItem*>>& GetContainerRef() { return Container; }
    const vector<vector<UItem*>>& GetContainerRef() const { return Container; }
    bool GetSellMode() {return bSellMode;}
    UItem* GetItem(Vector Index);
    UItem* GetRandomItem();
    
    bool SetSellMode(bool EnableSellMode);
    bool ToggleSellMode();
    
    void EnterNewShop(int ShopId);

    void SelectCursor();
    void TryBuyItem();
    void TrySellItem();


};
