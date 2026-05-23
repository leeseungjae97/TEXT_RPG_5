#include "MoveComponent.h"
#include "RenderManager.h"
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
	if(nullptr == PlayerPtr)
		PlayerPtr = dynamic_cast<Player*>(GetOwner());

	if (PlayerPtr == nullptr)
	{
		return;
	}

	PlayerPtr->SetPrevPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y }); 
	MoveElapsedTime += DeltaTime;

	if (MoveElapsedTime < MoveInterval)
	{
		return;
	}

	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::UP))
	{
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y - 1 });
		SetFacingDirection(EDirection::UP);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::DOWN))
	{
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
		SetFacingDirection(EDirection::DOWN);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::LEFT))
	{
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y});
		SetFacingDirection(EDirection::LEFT);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::RIGHT))
	{
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y});
		SetFacingDirection(EDirection::RIGHT);
		MoveElapsedTime = 0.0f;
	}
}
