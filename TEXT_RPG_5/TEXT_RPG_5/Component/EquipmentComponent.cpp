#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "../Player.h"
#include "../Enum/EquipmentTypeEnum.h"

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

bool UEquipmentComponent::Equip(UItem* Item, UItem*& OutDisplaced)
{
    OutDisplaced = nullptr;

    EquipmentType SlotType = Item->GetItemInfo().EquipSlot;
    if (SlotType == EquipmentType::NONE)
        return false;

    int Row = (int)SlotType;

    if (Container[Row][0] != nullptr)
    {
        OutDisplaced = Container[Row][0];
        RemoveEquipEffect(OutDisplaced);
    }

    Container[Row][0] = Item;
    ApplyEquipEffect(Item);
    return true;
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

WeaponType UEquipmentComponent::GetCurrentWeaponType() const
{
    int WeaponRow = (int)EquipmentType::Weapon;
    
    if (Container[WeaponRow][0] == nullptr)
        return WeaponType::NONE;
    
    return Container[WeaponRow][0]->GetItemInfo().AttackType;
}

void UEquipmentComponent::ApplyEquipEffect(UItem* Item)
{
    if (PlayerPtr == nullptr) return;

    const FItemInfo& Info = Item->GetItemInfo();
    const int Amount = static_cast<int>(round(Info.EffectAmount * GetRarityStatMultiplier(Info.Rarity)));
    switch (Info.EffectType)
    {
    case StatType::HP:    PlayerPtr->SetHP(PlayerPtr->GetHP() + Amount);          break;
    case StatType::MaxHP: PlayerPtr->SetMax_HP(PlayerPtr->GetMax_HP() + Amount);  break;
    case StatType::Power: PlayerPtr->SetPower(PlayerPtr->GetPower() + Amount);    break;
    default: break;
    }
}

void UEquipmentComponent::RemoveEquipEffect(UItem* Item)
{
    if (PlayerPtr == nullptr) return;

    const FItemInfo& Info = Item->GetItemInfo();
    const int Amount = static_cast<int>(round(Info.EffectAmount * GetRarityStatMultiplier(Info.Rarity)));
    switch (Info.EffectType)
    {
    case StatType::HP:    PlayerPtr->SetHP(PlayerPtr->GetHP() - Amount);          break;
    case StatType::MaxHP: PlayerPtr->SetMax_HP(PlayerPtr->GetMax_HP() - Amount);  break;
    case StatType::Power: PlayerPtr->SetPower(PlayerPtr->GetPower() - Amount);    break;
    default: break;
    }
}
