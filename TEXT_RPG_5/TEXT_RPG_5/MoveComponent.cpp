#include "MoveComponent.h"
#include "InputManager.h"
#include "Object.h"
#include "Player.h"

UMoveComponent::UMoveComponent(AObject* InOwner)
	: UComponent(InOwner)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	this->FacingDirection = EDirection::NONE;
}

UMoveComponent::~UMoveComponent()
{

}

EDirection UMoveComponent::GetFacingDirection()
{
	return FacingDirection;
}

void UMoveComponent::SetFacingDirection(EDirection Str)
{
	this->FacingDirection = Str;
}

void UMoveComponent::Tick(float DeltaTime)
{
	PlayerPtr->SetPrevPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y }); 

	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::UP))
	{
		if (PlayerPtr != nullptr)
		{
			PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y - 1 });
			SetFacingDirection(EDirection::UP);
			Sleep(120);
		}
	}
	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::DOWN))
	{
		if (PlayerPtr != nullptr)
		{
			PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
			SetFacingDirection(EDirection::DOWN);
			Sleep(120);
		}
	}
	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::LEFT))
	{
		if (PlayerPtr != nullptr)
		{
			PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y});
			SetFacingDirection(EDirection::LEFT);
			Sleep(120);
		}
	}
	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::RIGHT))
	{
		if (PlayerPtr != nullptr)
		{
			PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y});
			SetFacingDirection(EDirection::RIGHT);
			Sleep(120);
		}
	}
}
