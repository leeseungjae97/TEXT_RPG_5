//ShopManager.cpp

#include "ShopManager.h"
#include "ItemManager.h"
#include "../Item/ItemDB.h"
#include "../Item/Item.h"
#include "../Component/InventoryComponent.h"

/* for template
for (int y = 0; y < Container.size(); ++y)
{
    for (int x = 0; x < MaxColumn; ++x)
    {
        if (Container[y][x] != nullptr)
        {
            >>Execute<<
        }
    }
}
*/

ShopManager::ShopManager()
{
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));  // MaxRow 기준으로 초기화
    RestoreShop();
}

ShopManager::~ShopManager()
{
    for (int y = 0; y < Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            delete Container[y][x];
            Container[y][x] = nullptr;
        }
    }

    Container.clear();
}


void ShopManager::SetPlayerInventory(UInventoryComponent* Inventory)
{
    PlayerInventory = Inventory;
}

void ShopManager::RestoreShop()
{
    for (int y = 0; y < Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            delete Container[y][x];
        }
    }
    
    Container.clear();
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));  // MaxRow 기준으로 초기화

    for (int i = 0; i < NumberOfItems; ++i)
    {
        int x = i % MaxColumn;
        int y = i / MaxColumn;

        if (y < MaxRow)
        {
            Container[y][x] = GetRandomItem();
        }
    }
}
//테스트
void ShopManager::BeginPlay()
{
    // ShopPoses.resize(3);
    //
    // for (int i = 0 ; i < ShopPoses.size(); ++i)
    // {
    //     ShopPoses[i].X = 5 + i * 2;
    //     ShopPoses[i].Y = 5 + i * 2;
    // }
}


UItem* ShopManager::GetItem(Vector Index)
{
    if (Index.Y < 0 || Index.Y >= (int)Container.size())
    {
        return nullptr;
    }
    
    if (Index.X < 0 || Index.X >= MaxColumn)
    {
        return nullptr;
    }
    
    
    return Container[Index.Y][Index.X];
}


UItem* ShopManager::GetRandomItem()
{
    int n = rand() % 2;

    switch (n)
    {
    case 0:
        return ItemManager::GetInstance()->CreateItem(ItemId::HP_POTION);

    case 1:
        return ItemManager::GetInstance()->CreateItem(ItemId::STRENGTH_POTION);
    }

    return nullptr;
}


bool ShopManager::SetSellMode(bool EnableSellMode)
{
    if (EnableSellMode != bSellMode)
    {
        bSellMode = EnableSellMode;
        if (PlayerInventory != nullptr)
        {
            PlayerInventory->ResetCursor();
        }
        return bSellMode;
    }

    return bSellMode;
}

bool ShopManager::ToggleSellMode()
{
    bSellMode = !bSellMode;
    if (PlayerInventory != nullptr)
    {
        PlayerInventory->ResetCursor();
    }
    return bSellMode;
}



void ShopManager::SelectCursor()
{
    if (bSellMode == true)
    {
        TrySellItem();
    }
    else
    {
        TryBuyItem();
    }
}

void ShopManager::TryBuyItem()
{
    UItem* Item = GetItem(PlayerInventory->GetCursor());
    if (Item == nullptr)
    {
        return;
    }
    
    if (PlayerInventory->GetGold() >= Item->GetItemInfo().Price)
    {
        if (PlayerInventory->BuyItem(Item) == true )
        {
            Vector Cursor = PlayerInventory->GetCursor();
            Container[Cursor.Y][Cursor.X] = nullptr;
        }
        else
        {
            //인벤토리가 가득 찼다는 피드백
        }
    }
    else
    {
        //골드가 부족하다는 피드백
    }
}

void ShopManager::EnterNewShop(int ShopId)
{
    if (ShopId != LastVisitedShopId)
    {
        RestoreShop();
        LastVisitedShopId = ShopId;
    }
}

void ShopManager::TrySellItem()
{
    PlayerInventory->SellItem(PlayerInventory->GetItem(PlayerInventory->GetCursor()));
}
