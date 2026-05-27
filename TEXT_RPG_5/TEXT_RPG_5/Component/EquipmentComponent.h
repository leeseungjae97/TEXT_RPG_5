#pragma once

#include "../pch.h"
#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Item/Item.h"
#include "../Struct/Vector.h"

class Player;
class UInventoryComponent;

class UEquipmentComponent : public UComponent
{
    friend class AObject;

public:
    static constexpr ComponentType Type = ComponentType::EquipmentComponent;

protected:
    UEquipmentComponent(AObject* InOwner);
    UEquipmentComponent() = delete;

public:
    ~UEquipmentComponent();
    virtual void Tick(float DeltaTime) override;

    bool Equip(UItem* Item, UItem*& OutDisplaced);
    bool UnEquip(Vector Cursor, UInventoryComponent* Inventory);

    vector<vector<UItem*>>& GetContainer() { return Container; }
    UItem* GetItem(Vector Index);
    WeaponType GetCurrentWeaponType() const;

private:
    void ApplyEquipEffect(UItem* Item);
    void RemoveEquipEffect(UItem* Item);

    Player* PlayerPtr = nullptr;
    static constexpr int MaxRow = 4;
    static constexpr int MaxColumn = 1;
    vector<vector<UItem*>> Container;
};
