//Inventory.cpp

#include "InventoryComponent.h"
#include "../Item.h"
#include "../Manager/RenderManager.h"
#include "../Player.h"
#include "../Manager/InputManager.h"


UInventoryComponent::UInventoryComponent(AObject* InOwner)
    : UComponent(InOwner)
{
    PlayerPtr = dynamic_cast<Player*>(InOwner);
}

UInventoryComponent::~UInventoryComponent()
{

}

void UInventoryComponent::OpenInventory()
{
    int currentCount = 0;
    for (UItem* Item : Container)
    {
        ++currentCount;
        Item->printInfo();
    }
}


int UInventoryComponent::GetGold()
{
    return Gold;
}

void UInventoryComponent::AddGold(int Amount)
{
    Gold += Amount;
}

int UInventoryComponent::GetItemIndex(UItem* Item)
{
    auto it = find(Container.begin(), Container.end(), Item);
    if (it != Container.end())
        return it - Container.begin();
    else
        return -1;
}

UItem* UInventoryComponent::GetItem(int Index)
{
    return Container[Index];
}

void UInventoryComponent::AddItem(UItem* Item)
{
    Container.push_back(Item);
    RenderManager::GetInstance();
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{ 
    auto it = find(Container.begin(), Container.end(), Item);
    if (it != Container.end())
    {
        Container.erase(it);
        return true;
    }

    return false;
}

bool UInventoryComponent::UseRandomItem()
{
    int n = rand() % Container.size();
    UItem* Item = Container[n];

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



void UInventoryComponent::UpdateInventorySlot()
{

    memset(InventorySlot, 0, sizeof(InventorySlot));

    for (int i = 0; i < Container.size(); i++)
    {
        if(i<16)
        {
            InventorySlot[i / 4][i % 4] = Container[i];
        }

    }
   
}

UItem* UInventoryComponent::GetItemFromCursor()
{
    return InventorySlot[CurrentCursor.X][CurrentCursor.Y];
}

bool UInventoryComponent::UseCursorItem()
{
    if (UseItem(GetItemFromCursor()) == true)
    {
        UpdateInventorySlot();
    }
    return true;
}




map<int, UItem*> UInventoryComponent::GetQuickSlot()
{
    return QuickSlot;
}


void UInventoryComponent::RegisterOnQuickSlot(int Number, UItem* Item)
{
    QuickSlot[Number] = Item;
}

UItem* UInventoryComponent::GetItemFromQuickSlot(int Number)
{
    return QuickSlot[Number];
}

void UInventoryComponent::UseQuickSlot(int Number)
{
    UseItem(QuickSlot[Number]);
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
