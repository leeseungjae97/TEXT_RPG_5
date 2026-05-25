//ShopManager.h

#pragma once

#include "../Singleton.h"
#include "../pch.h"
#include "../Vector.h"


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


public:
    
    void SetPlayerInventory(UInventoryComponent* Inventory);
    void RestoreShop();

    vector<vector<UItem*>> GetContainer() {return Container;}
    bool GetSellMode() {return bSellMode;}
    UItem* GetItem(Vector Index);
    UItem* GetRandomItem();
    bool ToggleMode();
    
    void SelectCursor();
    void TryBuyItem();
    void TrySellItem();


};
