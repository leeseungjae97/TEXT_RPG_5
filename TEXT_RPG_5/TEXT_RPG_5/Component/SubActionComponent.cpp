#include "SubActionComponent.h"

#include "InventoryComponent.h"
#include "../Manager/InputManager.h"
#include "../Player.h"


void USubActionComponent::Tick(float DeltaTime)
{
	TimeCheck += DeltaTime;
	if (TimeCheck < CoolTime)
	{
		return;
	}
	
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_1))
	{
		UseSlotItem(0);
	} 
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_2))
	{
		UseSlotItem(1);
	}
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_3))
	{
		UseSlotItem(2);
	}
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_4))
	{
		UseSlotItem(3);
	}

}

void USubActionComponent::UseSlotItem(int NumKey) 
{
	Player* player = dynamic_cast<Player*>(GetOwner());
	if (player == nullptr)
	{
		return;
	}
	UInventoryComponent* IC = player->GetComponent<UInventoryComponent>();
	if (IC == nullptr)
	{
		return;
	}
	if (IC->GetItemFromQuickSlot(NumKey) != nullptr) // 퀵슬롯 비어있지 않으면 작동
	{
		TimeCheck = 0.0f;
		IC->UseItem(IC->GetItemFromQuickSlot(NumKey)); //use->remove->clearQucislot 이어짐
	}
}
