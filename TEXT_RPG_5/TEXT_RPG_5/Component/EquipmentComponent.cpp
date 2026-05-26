#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "../Player.h"
#include "../Enum/ItemIdEnum.h"

UEquipmentComponent::UEquipmentComponent(AObject* InOwner)
    : UComponent(InOwner)
{
    PlayerPtr = dynamic_cast<Player*>(InOwner);
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));
}

UEquipmentComponent::~UEquipmentComponent()
{
    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            delete Container[y][x];
            Container[y][x] = nullptr;
        }
        
    }
    Container.clear();
}

void UEquipmentComponent::Tick(float DeltaTime)
{
    if (nullptr == PlayerPtr)
        PlayerPtr = dynamic_cast<Player*>(GetOwner());
}

bool UEquipmentComponent::Equip(UItem* Item)
{
    for (int y = 0; y < MaxRow; ++y)
    {
        if (nullptr == Container[y][0])
        {
            Container[y][0] = Item;
            ApplyEquipEffect(Item);
            
            return true;
            
        }
        
        
    }
    return false;
}

bool UEquipmentComponent::UnEquip(Vector Cursor, UInventoryComponent* Inventory)
{
    if (Cursor.Y < 0 || Cursor.Y >= MaxRow)
        return false;

    UItem* Item = Container[Cursor.Y][0];
    if (nullptr == Item)
        return false;

    if (Inventory->IsFull())
        return false;

    RemoveEquipEffect(Item);
    Inventory->AddItem(Item);
    Container[Cursor.Y][0] = nullptr;
    return true;
}

UItem* UEquipmentComponent::GetItem(Vector Index)
{
    if (Index.Y < 0 || Index.Y >= MaxRow) return nullptr;
    if (Index.X != 0) return nullptr;
    return Container[Index.Y][0];
}

void UEquipmentComponent::ApplyEquipEffect(UItem* Item)
{
    if (PlayerPtr == nullptr)
        return;

    switch (Item->GetItemInfo().Id)
    {
    case ItemId::LONGSWORD:
        PlayerPtr->SetPower(PlayerPtr->GetPower() + Item->GetItemInfo().EffectAmount);
        break;
    }
}

void UEquipmentComponent::RemoveEquipEffect(UItem* Item)
{
    if (PlayerPtr == nullptr) return;

    switch (Item->GetItemInfo().Id)
    {
    case ItemId::LONGSWORD:
        PlayerPtr->SetPower(PlayerPtr->GetPower() - Item->GetItemInfo().EffectAmount);
        break;
    }
}
