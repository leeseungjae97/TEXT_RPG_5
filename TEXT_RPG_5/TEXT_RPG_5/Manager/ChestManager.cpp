//ChestManager.cpp

#include "ChestManager.h"
#include "ItemManager.h"
#include "../Define.h"
#include "../Item/Item.h"
#include "../Component/InventoryComponent.h"
#include "../Struct/ItemWeight.h"


static const vector<FItemWeight> ChestItemPool =
{
    { ItemId::LONGSWORD,        10 },
    { ItemId::BOW,              10 },
    { ItemId::AXE,              10 },
    { ItemId::STAFF,            10 },
    { ItemId::LEATHER_HELMET,    8 },
    { ItemId::LEATHER_ARMOR,     8 },
    { ItemId::EXPLORER_BOOTS,     8 },
    { ItemId::PLATE_HELMET,      4 },
    { ItemId::PLATE_ARMOR,       4 },
    { ItemId::BOOTS_OF_WIND,       4 },
    { ItemId::HP_POTION,        200 },
    { ItemId::STRENGTH_POTION,  100 },
};

ChestManager::ChestManager()
{
}

ChestManager::~ChestManager()
{
    ClearChests();
}

int ChestManager::MakeKey(Vector Pos)
{
    return Pos.Y * MAP_MAX_X + Pos.X;
}

void ChestManager::FillLoot(vector<vector<UItem*>>& Container)
{
    Container.assign(MaxRow, vector<UItem*>(MaxColumn, nullptr));

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

void ChestManager::ClearChests()
{
    for (auto& Entry : Loots)
    {
        for (auto& Row : Entry.second)
        {
            for (UItem*& Item : Row)
            {
                delete Item;
                Item = nullptr;
            }
        }
    }

    Loots.clear();
    ChestPoses.clear();
    ActiveKey = -1;
}

void ChestManager::SetPlayerInventory(UInventoryComponent* Inventory)
{
    PlayerInventory = Inventory;
}

void ChestManager::ResetRuntimeCache()
{
    PlayerInventory = nullptr;
    ActiveKey = -1;
}

void ChestManager::RegisterChest(Vector Pos)
{
    int Key = MakeKey(Pos);
    if (Loots.find(Key) != Loots.end())   // 이미 등록된 위치면 무시
        return;

    FillLoot(Loots[Key]);
    ChestPoses.push_back(Pos);
}

void ChestManager::BeginPlay()
{
    // 실제 배치는 맵 생성 시 MapManager가 RegisterChest로 등록한다.
    ClearChests();
}

bool ChestManager::OpenChestAt(Vector Pos)
{
    int Key = MakeKey(Pos);
    if (Loots.find(Key) == Loots.end())
        return false;

    ActiveKey = Key;
    return true;
}

vector<vector<UItem*>>& ChestManager::GetContainerRef()
{
    auto It = Loots.find(ActiveKey);
    if (It != Loots.end())
        return It->second;

    return EmptyContainer;
}

UItem* ChestManager::GetItem(Vector Index)
{
    vector<vector<UItem*>>& Container = GetContainerRef();

    if (Index.Y < 0 || Index.Y >= (int)Container.size())
        return nullptr;

    if (Index.X < 0 || Index.X >= MaxColumn)
        return nullptr;

    return Container[Index.Y][Index.X];
}

UItem* ChestManager::GetRandomItem()
{
    return ItemManager::GetInstance()->CreateRandomItem(ChestItemPool);
}

bool ChestManager::StoreItem(UItem* Item)
{
    if (Item == nullptr)
        return false;

    vector<vector<UItem*>>& Container = GetContainerRef();
    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            if (Container[y][x] == nullptr)
            {
                Container[y][x] = Item;
                return true;
            }
        }
    }

    return false;
}

void ChestManager::RemoveAt(Vector Index)
{
    vector<vector<UItem*>>& Container = GetContainerRef();

    if (Index.Y < 0 || Index.Y >= (int)Container.size())
        return;

    if (Index.X < 0 || Index.X >= MaxColumn)
        return;

    Container[Index.Y][Index.X] = nullptr;
}
