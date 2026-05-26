#include "SubActionComponent.h"

#include "InventoryComponent.h"
#include "../Manager/InputManager.h"
#include "../Player.h"


void SubActionComponent::Tick(float DeltaTime)
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

}

void SubActionComponent::UseSlotItem(int NumKey) 
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
