#include "CombatComponent.h"
#include "MoveComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/RenderManager.h"
#include "../Player.h"

UCombatComponent::UCombatComponent(AObject* InOwner)
	: UComponent(InOwner)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	MoveComponentPtr = PlayerPtr->GetComponent<UMoveComponent>();
	this->TotalTime = 0.0f;
	this->DelayTime = 2.0f;
}

UCombatComponent::~UCombatComponent()
{

}

vector<Vector> UCombatComponent::GetAttackValue()
{
	return AttackValue;
}

void UCombatComponent::Tick(float DeltaTime)
{
	if (nullptr == MoveComponentPtr)
		return;

	if (PlayerPtr != nullptr)
	{
		if (InputManager::GetInstance()->IsKeyDown(eKeyCode::Z) && (TotalTime >= DelayTime))
		{
			if (!(PlayerPtr->GetIsAttack()))
			{
				AttackValue.clear();
				PlayerPtr->SetIsAttack(true);

				if (MoveComponentPtr->GetFacingDirection() == EDirection::UP)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y - 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::DOWN)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y + 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y + 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::RIGHT)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y + 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::LEFT)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y + 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::NONE)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
				}
			}
			TotalTime = 0.0f;
			
		}
		else
		{
			TotalTime += DeltaTime;
			RenderManager::GetInstance()->AddRender(2, 2, "타이머 : " + to_string(DeltaTime));
		}
		
	}
}
