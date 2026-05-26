#include "SubActionComponent.h"
#include "../Manager/InputManager.h"
#include "../Player.h"

void SubActionComponent::Tick(float DeltaTime)
{
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_1))
	{
		//UseSlotItem(0);
	} 

	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_2))
	{
		//UseSlotItem(2);
	}

}

void SubActionComponent::UseSlotItem(int NumKey) 
{
	if (Player* player = dynamic_cast<Player*>(GetOwner())) 
	{
		/*if (InventoryComponent* IC = player->GetComponent<InventoryComponent>()) 
		{
			if()

			IC->  ->Use(player);
			IC->RemoveItem()
		}*/
	}

}
