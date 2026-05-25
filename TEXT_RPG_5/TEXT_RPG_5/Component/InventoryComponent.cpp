//Inventory.cpp

#include "InventoryComponent.h"
#include "../Item.h"
#include "../Player.h"
#include "../Manager/InputManager.h"
#include "../Manager/RenderManager.h"
#include "../Manager/ShopManager.h"


UInventoryComponent::UInventoryComponent(AObject* InOwner)
    : UComponent(InOwner)
{
    PlayerPtr = dynamic_cast<Player*>(InOwner);
    
    
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));

   
}

UInventoryComponent::~UInventoryComponent()
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

void UInventoryComponent::OpenInventory()
{
    /*int currentCount = 0;
    for (vector<UItem*> Cont : Container)
    {
        for (UItem* Item : Cont)
        {
            ++currentCount;
            if (Item != nullptr)
                Item->printInfo();
        }
    }*/
    
    bOpenedInventory = true;
    
}


Vector UInventoryComponent::GetItemIndex(UItem* Item)
{
    for (int y = 0; y < Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            if (Container[y][x] == Item)
            {
                return { x, y };
            }
        }
    }

    return { -1, -1 };
}


UItem* UInventoryComponent::GetItem(Vector Index)
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


bool UInventoryComponent::IsFull()
{
    if ((int)Container.size() < MaxRow) return false;
    
    for (int y = 0; y < Container.size(); ++y)
        for (int x = 0; x < MaxColumn; ++x)
            if (Container[y][x] == nullptr) return false;
    
    return true;
}


bool UInventoryComponent::AddItem(UItem* Item)
{
    
    if (IsFull())
    {
        //인벤토리 가득 찼다는 피드백 넣어주기
        return false;
    }
    
    for (int y = 0; y < Container.size(); ++y)
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
    

    vector<UItem*> NewRow(MaxColumn, nullptr);
    NewRow[0] = Item;
    Container.push_back(NewRow);
    return true;
    
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
    
    ClearQuickSlot(Item);
    
    
    for (int y = 0; y < Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            if (Container[y][x] == Item)
            {
                delete Container[y][x];
                Container[y][x] = nullptr;

                return true;
            }
        }
    }

    return false;
}

bool UInventoryComponent::UseRandomItem()
{
    int y = rand() % Container.size();
    int x = rand() % Container[y].size();
    UItem* Item = Container[y][x];

    if (Item != nullptr && Item->GetItemInfo().Type == ItemType::Usable)
    {
        Item->Use(PlayerPtr);
        RemoveItem(Item);
        return true;
    }
    else
    {
        return false;
    }
    
}

bool UInventoryComponent::UseItem(UItem* Item)
{
    if (Item == nullptr)
    {
        return false;
    }
    
    if (Item->GetItemInfo().Type == ItemType::Usable)
    {
        Item->Use(PlayerPtr);
        RemoveItem(Item);
        return true;
    }
    else
    {
        return false;
    }
}



void UInventoryComponent::SelectCursor()
{
    if (GetItem(GetCursor()) != nullptr)
    {
        if (bOnShop == true)
        {
            ShopManager::GetInstance()->SelectCursor();
        }
        else
        {
            UseItem(GetItem(GetCursor()));
        }
    }
}

Vector UInventoryComponent::CursorUp()
{
    if (CurrentCursor.Y > 0)
        CurrentCursor.Y -= 1;
    return CurrentCursor;
}

Vector UInventoryComponent::CursorDown()
{
    if (CurrentCursor.Y < Container.size() - 1)
        CurrentCursor.Y += 1;
    return CurrentCursor;
}

Vector UInventoryComponent::CursorLeft()
{
    if (CurrentCursor.X > 0)
        CurrentCursor.X -= 1;
    return CurrentCursor;
}

Vector UInventoryComponent::CursorRight()
{
    if (CurrentCursor.X < MaxColumn - 1)
        CurrentCursor.X += 1;
    return CurrentCursor;
}



void UInventoryComponent::RegisterOnQuickSlot(int Number)
{
    if (GetItem(GetCursor()) != nullptr)
    {
        QuickSlot[Number] = GetItem(GetCursor());
    }
}


void UInventoryComponent::ClearQuickSlot(UItem* Item)
{
    for (auto& slot : QuickSlot)
    {
        if (slot.second == Item)
            slot.second = nullptr;
    }
}



bool UInventoryComponent::BuyItem(UItem* Item)
{
    if (Gold >= Item->GetItemInfo().Price)
    {
        if (AddItem(Item))
        {
            AddGold(-(Item->GetItemInfo().Price));
            return true;
        }
    }
    
    return false;
//돈부족 및 가방 가득 찼다는 피드백은 상점의 TryButItem이 처리 중.
}


void UInventoryComponent::SellItem(UItem* Item)
{
    if (RemoveItem(Item))
        AddGold(Item->GetItemInfo().Price);
}

void UInventoryComponent::Tick(float DeltaTime)
{
    if (nullptr == PlayerPtr)
        PlayerPtr = dynamic_cast<Player*>(GetOwner());

    if (PlayerPtr == nullptr)
    {
        return;
    }
}
