//CraftingManager.cpp

#include "CraftingManager.h"
#include "ItemManager.h"
#include "../Item/ItemDB.h"
#include "../Item/Item.h"
#include "../Component/InventoryComponent.h"
#include "../Struct/Recipe.h"


static const vector<FRecipe> RecipeBook =
{
    { ItemId::LEATHER_ARMOR,  { ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER } },
    { ItemId::LEATHER_HELMET, { ItemId::GOBLIN_LEATHER, ItemId::SLIME_JELLY } },
    { ItemId::LEATHER_BOOTS,  { ItemId::GOBLIN_LEATHER, ItemId::ORC_TUSK } },
    { ItemId::AXE,            { ItemId::ORC_TUSK, ItemId::LONGSWORD } },
    { ItemId::FLAME_POTION,   { ItemId::SPIDER_EYE, ItemId::STRENGTH_POTION } },
    { ItemId::PLATE_HELMET,   { ItemId::ORC_TUSK, ItemId::GOBLIN_LEATHER, ItemId::SLIME_JELLY } },
};

CraftingManager::CraftingManager()
{
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));
}

CraftingManager::~CraftingManager()
{
    ClearContainer();
    Container.clear();
}


void CraftingManager::ClearContainer()
{
    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            delete Container[y][x];
            Container[y][x] = nullptr;
        }
    }
}


void CraftingManager::SetPlayerInventory(UInventoryComponent* Inventory)
{
    PlayerInventory = Inventory;
}


void CraftingManager::ResetRuntimeCache()
{
    ClearContainer();
    PlayerInventory = nullptr;
}


void CraftingManager::BeginPlay()
{
}


UItem* CraftingManager::GetItem(Vector Index)
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


const FRecipe* CraftingManager::GetRecipeByResult(ItemId Result)
{
    for (const FRecipe& Recipe : RecipeBook)
    {
        if (Recipe.Result == Result)
            return &Recipe;
    }

    return nullptr;
}


bool CraftingManager::HasIngredients(const FRecipe& Recipe)
{
    if (PlayerInventory == nullptr)
        return false;

    for (ItemId Ingredient : Recipe.Ingredients)
    {
        int Required = 0;
        for (ItemId Other : Recipe.Ingredients)
            if (Other == Ingredient) ++Required;

        if (PlayerInventory->CountItemById(Ingredient) < Required)
            return false;
    }

    return true;
}


bool CraftingManager::OpenCraftingFor(ItemId Material)
{
    ClearContainer();
    Container.clear();
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));

    int Index = 0;
    for (const FRecipe& Recipe : RecipeBook)
    {
        bool UsesMaterial = false;
        for (ItemId Ingredient : Recipe.Ingredients)
        {
            if (Ingredient == Material)
            {
                UsesMaterial = true;
                break;
            }
        }

        if (!UsesMaterial)
            continue;

        int x = Index % MaxColumn;
        int y = Index / MaxColumn;
        if (y >= MaxRow)
            break;

        Container[y][x] = ItemManager::GetInstance()->CreateItem(Recipe.Result);
        ++Index;
    }

    return Index > 0;
}


bool CraftingManager::SelectCursor()
{
    return TryCraftItem();
}


bool CraftingManager::TryCraftItem()
{
    if (PlayerInventory == nullptr)
        return false;
    
    UItem* Result = GetItem(PlayerInventory->GetCursor());
    if (Result == nullptr)
        return false;
    
    ItemId ResultId = Result->GetItemInfo().Id;
    const FRecipe* Recipe = GetRecipeByResult(ResultId);
    if (Recipe == nullptr)
        return false;
    
    if (HasIngredients(*Recipe) == false)
    {
        
        return false;
        
    }
    
    for (ItemId Ingredient : Recipe->Ingredients)
    {
        UItem* Material = PlayerInventory->FindItemById(Ingredient);
        PlayerInventory->RemoveItem(Material);
        
    }
    
    
    return PlayerInventory->AcquireItem(ItemManager::GetInstance()->CreateItem(ResultId));
}
