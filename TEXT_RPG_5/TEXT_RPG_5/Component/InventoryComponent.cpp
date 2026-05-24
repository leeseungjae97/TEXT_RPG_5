//Inventory.cpp

#include "InventoryComponent.h"
#include "../Item.h"
#include "../Player.h"
#include "../Manager/InputManager.h"


UInventoryComponent::UInventoryComponent(AObject* InOwner)
    : UComponent(InOwner)
{
    PlayerPtr = dynamic_cast<Player*>(InOwner);
    
    
    Container.resize(1, vector<UItem*>(4, nullptr));

   
}

UInventoryComponent::~UInventoryComponent()
{

}

void UInventoryComponent::OpenInventory()
{
    int currentCount = 0;
    for (vector<UItem*> Cont : Container)
    {
        for (UItem* Item : Cont)
        {
            ++currentCount;
            Item->printInfo();
        }
    }
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
    return Container[Index.X][Index.Y];
}

void UInventoryComponent::AddItem(UItem* Item)
{
    for (int y = 0; y < Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            if (Container[y][x] == nullptr)
            {
                Container[y][x] = Item;
                
                return;
            }
        }
    }
    
    vector<UItem*> NewRow(MaxColumn, nullptr);

    NewRow[0] = Item;

    Container.push_back(NewRow);
    
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
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

    if (Item->Type == ItemType::Usable)
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

    if (Item->Type == ItemType::Usable)
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


void UInventoryComponent::BuyItem(UItem* Item)
{
    if (Gold >= Item->Price)
    {
        AddGold(-(Item->Price));
        AddItem(Item);
    }
    else
    {
        
    }
}


void UInventoryComponent::SellItem(UItem* Item)
{
    if (RemoveItem(Item))
        AddGold(Item->Price);
}

void UInventoryComponent::Tick(float DeltaTime)
{
    if (nullptr == PlayerPtr)
        PlayerPtr = dynamic_cast<Player*>(GetOwner());

    if (PlayerPtr == nullptr)
    {
        return;
    }

    PlayerPtr->SetPrevPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y });
    MoveElapsedTime += DeltaTime;

    if (MoveElapsedTime < MoveInterval)
    {
        return;
    }

    if (InputManager::GetInstance()->IsKeyDown(eKeyCode::UP))
    {
        PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y - 1 });
        MoveElapsedTime = 0.0f;
    }
    else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::DOWN))
    {
        PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
        MoveElapsedTime = 0.0f;
    }
    else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::LEFT))
    {
        PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y });
        MoveElapsedTime = 0.0f;
    }
    else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::RIGHT))
    {
        PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y });
        MoveElapsedTime = 0.0f;
    }
}
