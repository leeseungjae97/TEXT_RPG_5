//Inventory.cpp

#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "../Item/Item.h"
#include "../Item/ItemDB.h"
#include "../Manager/RenderManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ShopManager.h"
#include "../Manager/ItemManager.h"
#include "../Player.h"


UInventoryComponent::UInventoryComponent(AObject* InOwner)
    : UComponent(InOwner)
{
    PlayerPtr = dynamic_cast<Player*>(InOwner);

    Container.resize(MaxRow, vector<UItem*>(MaxColumn, nullptr));
    QuickSlot.resize(4, nullptr);
}

UInventoryComponent::~UInventoryComponent()
{
    for (int y = 0; y < Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            delete Container[y][x];
            Container[y][x] = nullptr;
        }
    }

    Container.clear();
}

void UInventoryComponent::OpenInventory()
{
    bOpenedInventory = true;
}

void UInventoryComponent::OpenShop()
{
    SetOnShop(true);
}

void UInventoryComponent::CloseInventory()
{
    bOpenedInventory = false;
    bOnEquipment = false;
    if (bOnShop)
    {
        bOnShop = false;
        ShopManager::GetInstance()->SetSellMode(false);
    }
    ResetCursor();
}


Vector UInventoryComponent::GetItemIndex(UItem* Item)
{
    if (nullptr == Item)
    {
        return { -1, -1 };
    }

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


bool UInventoryComponent::IsFull()
{
    if ((int)Container.size() < MaxRow) return false;

    for (int y = 0; y < Container.size(); ++y)
        for (int x = 0; x < MaxColumn; ++x)
            if (Container[y][x] == nullptr) return false;

    return true;
}


bool UInventoryComponent::AddItem(UItem* Item)
{
    if (IsFull())
    {
        //인벤토리 가득 찼다는 피드백 넣어주기
        return false;
    }

    for (int y = 0; y < Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            if (Container[y][x] == nullptr)
            {
                Container[y][x] = Item;
                return true;
            }
        }
    }

    vector<UItem*> NewRow(MaxColumn, nullptr);
    NewRow[0] = Item;
    Container.push_back(NewRow);
    return true;
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
    ClearQuickSlot(Item);

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

    if (Item != nullptr && Item->GetItemInfo().Type == ItemType::Usable)
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
    if (Item == nullptr)
    {
        return false;
    }
    
    
    if (Item->GetItemInfo().Type == ItemType::Usable)
    {
        ItemId UsedId = Item->GetItemInfo().Id;

        int QuickSlotIndex = -1;
        for (int i = 0; i < (int)QuickSlot.size(); ++i)
        {
            if (QuickSlot[i] == Item)
            {
                QuickSlotIndex = i;
                break;
            }
        }

        Item->Use(PlayerPtr);
        RemoveItem(Item);

        if (QuickSlotIndex != -1)
            QuickSlot[QuickSlotIndex] = FindItemById(UsedId);

        return true;
    }
    else
    {
        return false;
    }
}


void UInventoryComponent::SelectCursor()
{
    //장비창에 커서 있을 경우 입력 넘기기
    if (bOnEquipment)
    {
        UEquipmentComponent* EC = PlayerPtr->GetComponent<UEquipmentComponent>();
        if (EC) EC->UnEquip(CurrentCursor, this);
        return;
    }
    
    
    UItem* Item = GetItem(GetCursor());
    if (Item == nullptr) return;
    
    if (bOnShop)
    {
        ShopManager::GetInstance()->SelectCursor();
    }
    else if (Item->GetItemInfo().Type == ItemType::Equipment)
    {
        UEquipmentComponent* EC = PlayerPtr->GetComponent<UEquipmentComponent>();
        if (EC && EC->Equip(Item))
            DetachItem(Item);
    }
    else
    {
        UseItem(Item);
    }
}


//장비템을 인벤토리와 장비창 사이에서 옮길 때 사용 (컨테이너에서만 제거하고 포인터 살려두기)
UItem* UInventoryComponent::DetachItem(UItem* Item)
{
    ClearQuickSlot(Item);
    
    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            if (Container[y][x] == Item)
            {
                Container[y][x] = nullptr;
                return Item;
            }
        }
        
    }
    
    return nullptr;
}

Vector UInventoryComponent::CursorUp()
{
    if (CurrentCursor.Y > 0)
        CurrentCursor.Y -= 1;
    
    return CurrentCursor;
}

Vector UInventoryComponent::CursorDown()
{
    if (CurrentCursor.Y < Container.size() - 1)
        CurrentCursor.Y += 1;
    
    return CurrentCursor;
}

Vector UInventoryComponent::CursorLeft()
{
    if (bOnEquipment)
    {
        bOnEquipment = false;
        CurrentCursor = { MaxColumn - 1, 0 };
        return CurrentCursor;
    }
    
    if (CurrentCursor.X > 0)
        CurrentCursor.X -= 1;
    
    
    return CurrentCursor;
}

Vector UInventoryComponent::CursorRight()
{
    if (bOnEquipment)
        return CurrentCursor;

    
    if (CurrentCursor.X < MaxColumn - 1)
    {
        CurrentCursor.X += 1;
    }
    else if (!bOnShop)
    {
        bOnEquipment = true;
        CurrentCursor = { 0, 0 };
    }
    
    return CurrentCursor;
}


void UInventoryComponent::RegisterOnQuickSlot(int Number)
{
    UItem* Item = GetItem(GetCursor());
    if (Item == nullptr) return;
    if (Item->GetItemInfo().Type != ItemType::Usable) return;
    
    ItemId Id = Item->GetItemInfo().Id;
    for (int i = 0; i < (int)QuickSlot.size(); ++i)
    {
        if (i != Number && QuickSlot[i] != nullptr && QuickSlot[i]->GetItemInfo().Id == Id)
            QuickSlot[i] = nullptr;
    }
    
    QuickSlot[Number] = Item;
}

UItem* UInventoryComponent::FindItemById(ItemId Id)
{
    for (int y = 0; y < (int)Container.size(); ++y)
    {
        for (int x = 0; x < MaxColumn; ++x)
        {
            if (Container[y][x] != nullptr && Container[y][x]->GetItemInfo().Id == Id)
                return Container[y][x];
        }
    }
    
    return nullptr;
}


void UInventoryComponent::ClearQuickSlot(UItem* Item)
{
    for (auto& slot : QuickSlot)
    {
        if (slot == Item)
        {
            slot = nullptr;
            break;
        }
    }
}


bool UInventoryComponent::BuyItem(UItem* Item)
{
    if (Gold >= Item->GetItemInfo().Price)
    {
        if (AddItem(Item))
        {
            AddGold(-(Item->GetItemInfo().Price));
            return true;
        }
    }

    return false;
//돈부족 및 가방 가득 찼다는 피드백은 상점의 TryButItem이 처리 중.
}


void UInventoryComponent::SellItem(UItem* Item)
{
    int Price = Item->GetItemInfo().Price;
    if (RemoveItem(Item))
        AddGold(Price);
}

bool UInventoryComponent::SetOnShop(bool OnShop)
{
    bOnShop = OnShop;
    if (bOnShop)
        CloseEquipmentPanel();
    else
        ShopManager::GetInstance()->SetSellMode(false);
    return bOnShop;
}

bool UInventoryComponent::ToggleOnShop()
{
    bOnShop = !bOnShop;
    if (bOnShop)
        CloseEquipmentPanel();
    else
        ShopManager::GetInstance()->SetSellMode(false);
    return bOnShop;
}


void UInventoryComponent::Tick(float DeltaTime)
{
    if (nullptr == PlayerPtr)
        PlayerPtr = dynamic_cast<Player*>(GetOwner());

    if (nullptr == PlayerPtr)
        return;

    InputManager* Input = InputManager::GetInstance();
    
    //퀵슬롯 사용(인벤토리 꺼져있을 때)
    if (!bOpenedInventory)
    {
        if (Input->IsKeyTap(KeyCode::_1)) UseQuickSlot(0);
        if (Input->IsKeyTap(KeyCode::_2)) UseQuickSlot(1);
        if (Input->IsKeyTap(KeyCode::_3)) UseQuickSlot(2);
        if (Input->IsKeyTap(KeyCode::_4)) UseQuickSlot(3);
        return;
    }
    
    //이 밑으로는 다 bOpenedInventory가 true일 때만(인벤토리 열었을 경우에만) 작동.
    
    //아이템 및 골드 지급(디버그용)
    if (Input->IsKeyTap(KeyCode::B))
    {
        AddItem(ItemManager::GetInstance()->CreateItem(ItemId::HP_POTION));
        AddItem(ItemManager::GetInstance()->CreateItem(ItemId::STRENGTH_POTION));
        AddItem(ItemManager::GetInstance()->CreateItem(ItemId::LONGSWORD));
        AddGold(500);
    }
    
    
    
    //커서 이동 및 결정(사용 or 판매 or 구매) 버튼
    if (Input->IsKeyTap(KeyCode::UP))    CursorUp();
    if (Input->IsKeyTap(KeyCode::DOWN))  CursorDown();
    if (Input->IsKeyTap(KeyCode::LEFT))  CursorLeft();
    if (Input->IsKeyTap(KeyCode::RIGHT)) CursorRight();
    if (Input->IsKeyTap(KeyCode::Z))     SelectCursor();
    
    
    //누르면 샵 진입 or 퇴장 토글
    if (Input->IsKeyTap(KeyCode::X))
    {
        if (ToggleOnShop())
            ShopManager::GetInstance()->SetPlayerInventory(this);
    }
    
    //샵에 진입했을 때, 이걸 누르면 SellMode 활성화.
    if (bOnShop && Input->IsKeyTap(KeyCode::C))
    {
        if (!ShopManager::GetInstance()->GetSellMode())
            ShopManager::GetInstance()->SetSellMode(true);
    }
    
    //
    if (bOnShop && Input->IsKeyTap(KeyCode::V))
    {
        if (ShopManager::GetInstance()->GetSellMode())
            ShopManager::GetInstance()->SetSellMode(false);
    }
    
    //퀵슬롯에 등록 (인벤토리 켜져있을 때)
    if (Input->IsKeyTap(KeyCode::_1)) RegisterOnQuickSlot(0);
    if (Input->IsKeyTap(KeyCode::_2)) RegisterOnQuickSlot(1);
    if (Input->IsKeyTap(KeyCode::_3)) RegisterOnQuickSlot(2);
    if (Input->IsKeyTap(KeyCode::_4)) RegisterOnQuickSlot(3);
}
