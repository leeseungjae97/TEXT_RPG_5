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
		

	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::_1))
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
	TimeCheck = 0.0f;
	IC->UseItem(IC->GetItemFromQuickSlot(NumKey)); //use->remove->clearQucislot 이어짐
}
