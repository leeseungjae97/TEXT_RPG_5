//CraftingManager.cpp

#include "CraftingManager.h"
#include "ItemManager.h"
#include "../Item/ItemDB.h"
#include "../Item/Item.h"
#include "../Component/InventoryComponent.h"
#include "../Struct/Recipe.h"


static const vector<FRecipe> RecipeBook =
{
    // Usable
    { ItemId::HP_POTION,               { ItemId::SLIME_JELLY, ItemId::SLIME_JELLY } },
    { ItemId::HP_POTION,               { ItemId::SLIME_JELLY, ItemId::GOBLIN_LEATHER } },
    { ItemId::STRENGTH_POTION,         { ItemId::HP_POTION, ItemId::GOBLIN_LEATHER } },
    { ItemId::STRENGTH_POTION,         { ItemId::HP_POTION, ItemId::SLIME_JELLY } },
    { ItemId::STRENGTH_POTION,         { ItemId::HP_POTION, ItemId::ORC_TUSK } },
    { ItemId::FLAME_POTION,            { ItemId::STRENGTH_POTION, ItemId::HP_POTION, ItemId::SPIDER_EYE } },
    { ItemId::FLAME_POTION,            { ItemId::HP_POTION, ItemId::FIRE_WALL_SCROLL } },
    { ItemId::FLAME_POTION,            { ItemId::DRAGON_HEART } },
    { ItemId::FIRE_WALL_SCROLL,        { ItemId::GOBLIN_LEATHER, ItemId::HP_POTION, ItemId::SPIDER_EYE } },
    { ItemId::FIRE_WALL_SCROLL,        { ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER } },
    { ItemId::FIRE_WALL_SCROLL,        { ItemId::LIGHTNING_STRIKE_SCROLL, ItemId::SPIDER_EYE } },
    { ItemId::FIRE_WALL_SCROLL,        { ItemId::FLAME_POTION } },
    { ItemId::LIGHTNING_STRIKE_SCROLL, { ItemId::GOBLIN_LEATHER, ItemId::SLIME_JELLY, ItemId::STRENGTH_POTION } },
    { ItemId::LIGHTNING_STRIKE_SCROLL, { ItemId::FIRE_WALL_SCROLL, ItemId::ORC_TUSK } },
    { ItemId::LIGHTNING_STRIKE_SCROLL, { ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER } },

    // Weapon
    { ItemId::LONGSWORD,               { ItemId::ORC_TUSK, ItemId::ORC_TUSK } },
    { ItemId::LONGSWORD,               { ItemId::BOW, ItemId::ORC_TUSK } },
    { ItemId::LONGSWORD,               { ItemId::ORC_TUSK, ItemId::STRENGTH_POTION, ItemId::HP_POTION } },
    { ItemId::AXE,                     { ItemId::ORC_TUSK, ItemId::ORC_TUSK } },
    { ItemId::AXE,                     { ItemId::LONGSWORD, ItemId::ORC_TUSK } },
    { ItemId::AXE,                     { ItemId::ORC_TUSK, ItemId::STRENGTH_POTION, ItemId::STRENGTH_POTION } },
    { ItemId::STAFF,                   { ItemId::ORC_TUSK, ItemId::SPIDER_EYE } },
    { ItemId::STAFF,                   { ItemId::AXE, ItemId::SPIDER_EYE } },
    { ItemId::STAFF,                   { ItemId::FIRE_WALL_SCROLL, ItemId::LIGHTNING_STRIKE_SCROLL } },
    { ItemId::STAFF,                   { ItemId::FLAME_POTION, ItemId::SLIME_JELLY } },
    { ItemId::BOW,                     { ItemId::ORC_TUSK, ItemId::SPIDER_EYE } },
    { ItemId::BOW,                     { ItemId::STAFF, ItemId::SPIDER_EYE } },
    { ItemId::BOW,                     { ItemId::BOOTS_OF_WIND } },

    // Armor
    { ItemId::LEATHER_HELMET,          { ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER} },
    { ItemId::LEATHER_ARMOR,           { ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER, ItemId::ORC_TUSK } },
    { ItemId::PLATE_HELMET,            { ItemId::LEATHER_HELMET, ItemId::SPIDER_EYE, ItemId::AXE } },
    { ItemId::PLATE_HELMET,            { ItemId::PLATE_ARMOR, ItemId::SLIME_JELLY } },
    { ItemId::PLATE_ARMOR,             { ItemId::LEATHER_ARMOR, ItemId::SPIDER_EYE, ItemId::LONGSWORD, } },
    { ItemId::PLATE_ARMOR,             { ItemId::PLATE_HELMET, ItemId::GOBLIN_LEATHER } },

    // Boots
    { ItemId::EXPLORER_BOOTS,          { ItemId::GOBLIN_LEATHER, ItemId::GOBLIN_LEATHER, ItemId::SPIDER_EYE } },
    { ItemId::EXPLORER_BOOTS,          { ItemId::BOOTS_OF_WIND } },
    { ItemId::BOOTS_OF_WIND,           { ItemId::EXPLORER_BOOTS, ItemId::BOW, ItemId::STAFF } },
    { ItemId::BOOTS_OF_WIND,           { ItemId::SPIDER_EYE, ItemId::SPIDER_EYE, ItemId::SPIDER_EYE, } },
};

CraftingManager::CraftingManager()
{
    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));
    RecipeContainer.resize(MaxRow, vector<const FRecipe*>(MaxColumn, nullptr));
}

CraftingManager::~CraftingManager()
{
    ClearContainer();
    Container.clear();
    RecipeContainer.clear();
}


void CraftingManager::ClearContainer()
{
    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < (int)Container[y].size(); ++x)
        {
            delete Container[y][x];
            Container[y][x] = nullptr;
        }
    }

    for (int y = 0; y < (int)RecipeContainer.size(); ++y)
    {
        for (int x = 0; x < (int)RecipeContainer[y].size(); ++x)
        {
            RecipeContainer[y][x] = nullptr;
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
    RecipeContainer.clear();
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

const FRecipe* CraftingManager::GetRecipe(Vector Index)
{
    if (Index.Y < 0 || Index.Y >= (int)RecipeContainer.size())
    {
        return nullptr;
    }

    if (Index.X < 0 || Index.X >= MaxColumn)
    {
        return nullptr;
    }

    return RecipeContainer[Index.Y][Index.X];
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
    RecipeContainer.clear();
    RecipeContainer.resize(MaxRow, vector<const FRecipe*>(MaxColumn, nullptr));

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
        while (y >= (int)Container.size())
        {
            Container.push_back(vector<UItem*>(MaxColumn, nullptr));
            RecipeContainer.push_back(vector<const FRecipe*>(MaxColumn, nullptr));
        }

        Container[y][x] = ItemManager::GetInstance()->CreateItem(Recipe.Result);
        RecipeContainer[y][x] = &Recipe;
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
    
    const FRecipe* Recipe = GetRecipe(PlayerInventory->GetCursor());
    if (Recipe == nullptr)
        return false;
    
    ItemId ResultId = Recipe->Result;
    
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
