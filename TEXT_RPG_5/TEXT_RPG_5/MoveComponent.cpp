#include "MoveComponent.h"
#include "InputManager.h"
#include "Object.h"

UMoveComponent::UMoveComponent(AObject* InOwner)
	: UComponent(InOwner)
{
	FacingDirection = EDirection::None;
}

UMoveComponent::~UMoveComponent()
{

}

void UMoveComponent::SetFacingDirection(EDirection Str)
{
	this->FacingDirection = Str;
}
EDirection UMoveComponent::GetFacingDirection()
{
	return FacingDirection;
}

void UMoveComponent::Tick(float DeltaTime)
{
	// GetOwner() 변수로 받아라.
	AObject* OwnerObject= GetOwner();

	OwnerObject->SetPrevPosition({ OwnerObject->GetPosition().X, OwnerObject->GetPosition().Y }); //

	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::UP))
	{
		if (OwnerObject != nullptr)
		{
			OwnerObject->SetPosition({ OwnerObject->GetPosition().X, OwnerObject->GetPosition().Y - 1 });
			Sleep(120);
		}
	}
	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::DOWN))
	{
		if (OwnerObject != nullptr)
		{
			OwnerObject->SetPosition({ OwnerObject->GetPosition().X, OwnerObject->GetPosition().Y + 1 });
			Sleep(120);
		}
	}
	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::LEFT))
	{
		if (OwnerObject != nullptr)
		{
			OwnerObject->SetPosition({ OwnerObject->GetPosition().X - 1, OwnerObject->GetPosition().Y});
			Sleep(120);
		}
	}
	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::RIGHT))
	{
		if (OwnerObject != nullptr)
		{
			OwnerObject->SetPosition({ OwnerObject->GetPosition().X + 1, OwnerObject->GetPosition().Y});
			Sleep(120);
		}
	}

}
