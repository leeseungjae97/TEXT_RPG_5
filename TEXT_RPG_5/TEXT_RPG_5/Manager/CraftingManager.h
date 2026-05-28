//CraftingManager.h

#pragma once

#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/Vector.h"
#include "../Enum/ItemIdEnum.h"


class UInventoryComponent;
class UItem;
struct FRecipe;

class CraftingManager : public Singleton<CraftingManager>
{
public:

    CraftingManager();
    ~CraftingManager();

private:
    int MaxColumn = 4;
    int MaxRow = 4;
    vector<vector<UItem*>> Container;
    UInventoryComponent* PlayerInventory = nullptr;

    void ClearContainer();
    bool HasIngredients(const FRecipe& Recipe);


public:
    void SetPlayerInventory(UInventoryComponent* Inventory);
    void ResetRuntimeCache();
    void BeginPlay();

    vector<vector<UItem*>> GetContainer() {return Container;}
    vector<vector<UItem*>>& GetContainerRef() { return Container; }
    const vector<vector<UItem*>>& GetContainerRef() const { return Container; }
    UItem* GetItem(Vector Index);

    // UI를 위한 반환용
    const FRecipe* GetRecipeByResult(ItemId Result);
    
    bool OpenCraftingFor(ItemId Material);

    bool SelectCursor();
    bool TryCraftItem(); 


};
