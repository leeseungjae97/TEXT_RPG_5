//ShopManager.cpp

#include "ShopManager.h"
#include "ItemManager.h"
#include "../Item/ItemDB.h"
#include "../Item/Item.h"
#include "../Component/InventoryComponent.h"
#include "../Struct/ItemWeight.h"

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

static const vector<FItemWeight> ShopItemPool =
{
    { ItemId::HP_POTION,       150 },
    { ItemId::STRENGTH_POTION, 50 },
    { ItemId::LONGSWORD,       10 },
    { ItemId::BOW,             10 },
    { ItemId::AXE,             10 },
    { ItemId::STAFF,           10 },
    { ItemId::LEATHER_HELMET,   5 },
    { ItemId::LEATHER_ARMOR,    5 },
    { ItemId::EXPLORER_BOOTS,    5 },
    { ItemId::PLATE_HELMET,     2 },
    { ItemId::PLATE_ARMOR,      2 },
    { ItemId::BOOTS_OF_WIND,      2 },
};

static const vector<FItemWeight> ShopBonusPool =
{
    { ItemId::LONGSWORD, 30 },
    { ItemId::BOW,       30 },
    { ItemId::AXE,       30 },
    { ItemId::STAFF,     30 },
    { ItemId::LEATHER_ARMOR,     10 },
    { ItemId::LEATHER_HELMET,     10 },
    { ItemId::EXPLORER_BOOTS,     10 },
    { ItemId::PLATE_ARMOR,     10 },
    { ItemId::PLATE_HELMET,     10 },
    { ItemId::BOOTS_OF_WIND,     10 },
};

struct FShopBonusItemRule
{
    const vector<FItemWeight>* Pool;
    ERarity Rarity;
};

static const vector<FShopBonusItemRule> ShopBonusItemRules =
{
    { &ShopBonusPool, ERarity::Rare },
    { &ShopBonusPool, ERarity::Rare },
    { &ShopBonusPool, ERarity::Unique },
};

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

void ShopManager::ResetRuntimeCache()
{
    PlayerInventory = nullptr;
    CurrentCursor = { 0, 0 };
    bSellMode = false;
    LastVisitedShopId = -1;
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

    for (const FShopBonusItemRule& Rule : ShopBonusItemRules)
    {
        if (Rule.Pool != nullptr)
        {
            AddItemToNextEmptySlot(ItemManager::GetInstance()->CreateRandomItemWithRarity(*Rule.Pool, Rule.Rarity));
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
    return ItemManager::GetInstance()->CreateRandomItem(ShopItemPool);
}

bool ShopManager::AddItemToNextEmptySlot(UItem* Item)
{
    if (Item == nullptr)
    {
        return false;
    }

    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < (int)Container[y].size(); ++x)
        {
            if (Container[y][x] == nullptr)
            {
                Container[y][x] = Item;
                return true;
            }
        }
    }

    delete Item;
    return false;
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
    
    if (PlayerInventory->GetGold() >= Item->GetPrice())
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
