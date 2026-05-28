//EnhancementManager.cpp

#include "EnhancementManager.h"
#include "../Component/InventoryComponent.h"
#include "../Item/Item.h"

EnhancementManager::EnhancementManager()
{
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));
}

EnhancementManager::~EnhancementManager()
{
    ClearContainer();
    Container.clear();
}

void EnhancementManager::ClearContainer()
{
    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < (int)Container[y].size(); ++x)
        {
            Container[y][x] = nullptr;
        }
    }
}

void EnhancementManager::SetPlayerInventory(UInventoryComponent* Inventory)
{
    PlayerInventory = Inventory;
}

void EnhancementManager::ResetRuntimeCache()
{
    ClearContainer();
    Container.clear();
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));
    PlayerInventory = nullptr;
    EnhancementTarget = nullptr;
}

void EnhancementManager::BeginPlay()
{
}

UItem* EnhancementManager::GetItem(Vector Index)
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

bool EnhancementManager::IsMatchingEnhancementMaterial(const UItem* Item) const
{
    if (EnhancementTarget == nullptr || Item == nullptr || Item == EnhancementTarget)
    {
        return false;
    }

    const FItemInfo& TargetInfo = EnhancementTarget->GetItemInfo();
    const FItemInfo& MaterialInfo = Item->GetItemInfo();
    return TargetInfo.Type == ItemType::Equipment &&
        MaterialInfo.Type == ItemType::Equipment &&
        TargetInfo.Id == MaterialInfo.Id;
}

void EnhancementManager::RefreshEnhancementMaterials()
{
    ClearContainer();
    Container.clear();
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));

    if (PlayerInventory == nullptr || EnhancementTarget == nullptr)
    {
        return;
    }

    int Index = 0;
    vector<vector<UItem*>>& InventoryItems = PlayerInventory->GetContainer();
    for (int y = 0; y < (int)InventoryItems.size(); ++y)
    {
        for (int x = 0; x < (int)InventoryItems[y].size(); ++x)
        {
            UItem* Candidate = InventoryItems[y][x];
            if (!IsMatchingEnhancementMaterial(Candidate))
            {
                continue;
            }

            int TargetRow = Index / MaxColumn;
            int TargetColumn = Index % MaxColumn;
            while (TargetRow >= (int)Container.size())
            {
                Container.push_back(vector<UItem*>(MaxColumn, nullptr));
            }

            Container[TargetRow][TargetColumn] = Candidate;
            ++Index;
        }
    }
}

double EnhancementManager::GetEnhancementChancePercent(const UItem* Material) const
{
    if (EnhancementTarget == nullptr)
    {
        return 0.0;
    }

    if (Material != nullptr && Material->GetEnhancementCount() > EnhancementTarget->GetEnhancementCount())
    {
        return 100.0;
    }

    return 100.0 / (EnhancementTarget->GetEnhancementCount() + 1);
}

bool EnhancementManager::OpenEnhancementFor(UItem* Target)
{
    if (Target == nullptr || Target->GetItemInfo().Type != ItemType::Equipment)
    {
        return false;
    }

    EnhancementTarget = Target;
    RefreshEnhancementMaterials();
    return true;
}

bool EnhancementManager::SelectCursor()
{
    TryEnhanceItem();
    return false;
}

bool EnhancementManager::TryEnhanceItem()
{
    if (PlayerInventory == nullptr || EnhancementTarget == nullptr)
    {
        return false;
    }

    UItem* Material = GetItem(PlayerInventory->GetCursor());
    if (!IsMatchingEnhancementMaterial(Material))
    {
        return false;
    }

    const int TargetCount = EnhancementTarget->GetEnhancementCount();
    const int MaterialCount = Material->GetEnhancementCount();
    const double ChancePercent = GetEnhancementChancePercent(Material);
    const int ChanceBasisPoint = static_cast<int>(round(ChancePercent * 100.0));
    const bool bSuccess = (ChancePercent >= 100.0) || (rand() % 10000 < ChanceBasisPoint);

    if (bSuccess)
    {
        if (MaterialCount > TargetCount)
        {
            EnhancementTarget->MatchEnhancementCount(MaterialCount);
        }
        else
        {
            EnhancementTarget->ApplyEnhancementSuccess();
        }
    }

    PlayerInventory->RemoveItem(Material);
    RefreshEnhancementMaterials();
    PlayerInventory->ResetCursor();
    return bSuccess;
}
