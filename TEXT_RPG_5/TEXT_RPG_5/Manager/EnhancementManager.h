//EnhancementManager.h

#pragma once

#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/Vector.h"

class UInventoryComponent;
class UItem;

class EnhancementManager : public Singleton<EnhancementManager>
{
public:
    EnhancementManager();
    ~EnhancementManager();

private:
    int MaxColumn = 4;
    int MaxRow = 4;
    vector<vector<UItem*>> Container;
    UInventoryComponent* PlayerInventory = nullptr;
    UItem* EnhancementTarget = nullptr;

    void ClearContainer();
    bool IsMatchingEnhancementMaterial(const UItem* Item) const;
    void RefreshEnhancementMaterials();

public:
    void SetPlayerInventory(UInventoryComponent* Inventory);
    void ResetRuntimeCache();
    void BeginPlay();

    vector<vector<UItem*>> GetContainer() { return Container; }
    vector<vector<UItem*>>& GetContainerRef() { return Container; }
    const vector<vector<UItem*>>& GetContainerRef() const { return Container; }
    UItem* GetItem(Vector Index);

    const UItem* GetEnhancementTarget() const { return EnhancementTarget; }
    double GetEnhancementChancePercent(const UItem* Material = nullptr) const;

    bool OpenEnhancementFor(UItem* Target);
    bool SelectCursor();
    bool TryEnhanceItem();
};
