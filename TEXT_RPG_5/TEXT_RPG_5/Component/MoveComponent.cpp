#include "MoveComponent.h"
#include "../Manager/RenderManager.h"
#include "../Manager/InputManager.h"
#include "../Object.h"
#include "../Player.h"

UMoveComponent::UMoveComponent(AObject* InOwner)
	: UComponent(InOwner)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	this->FacingDirection = EDirection::UP;
	this->PreviousFacingDirection = EDirection::UP;
}

UMoveComponent::~UMoveComponent()
{

}

EDirection UMoveComponent::GetFacingDirection()
{
	return FacingDirection;
}

EDirection UMoveComponent::GetPreviousFacingDirection()
{
	return PreviousFacingDirection;
}

void UMoveComponent::SetFacingDirection(EDirection Str)
{
	if (Str == EDirection::NONE || Str == FacingDirection)
	{
		return;
	}

	PreviousFacingDirection = FacingDirection;
	this->FacingDirection = Str;
	TurnElapsedTime = 0.0f;
}

float UMoveComponent::GetMoveAlpha() const
{
	if (MoveInterval <= 0.0f)
	{
		return 1.0f;
	}

	return min(MoveElapsedTime / MoveInterval, 1.0f);
}

float UMoveComponent::GetTurnAlpha() const
{
	if (TurnDuration <= 0.0f)
	{
		return 1.0f;
	}

	float Alpha = min(TurnElapsedTime / TurnDuration, 1.0f);
	return Alpha * Alpha * (3.0f - 2.0f * Alpha);
}

void UMoveComponent::Tick(float DeltaTime)
{
	if(nullptr == PlayerPtr)
		PlayerPtr = dynamic_cast<Player*>(GetOwner());

	if (PlayerPtr == nullptr)
	{
		return;
	}

	MoveElapsedTime += DeltaTime;
	TurnElapsedTime += DeltaTime;

	if (MoveElapsedTime < MoveInterval)
	{
		return;
	}
	
	if (InputManager::GetInstance()->IsKeyDown(eKeyCode::UP))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, max(PlayerPtr->GetPosition().Y - 1, 0) });
		SetFacingDirection(EDirection::UP);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::DOWN))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
		SetFacingDirection(EDirection::DOWN);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::LEFT))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ max(PlayerPtr->GetPosition().X - 1, 0 ), PlayerPtr->GetPosition().Y});
		SetFacingDirection(EDirection::LEFT);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyDown(eKeyCode::RIGHT))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y});
		SetFacingDirection(EDirection::RIGHT);
		MoveElapsedTime = 0.0f;
	}
	
}
