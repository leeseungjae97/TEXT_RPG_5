#include "MoveComponent.h"

#include "InventoryComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/MapManager.h"
#include "../Manager/ShopManager.h"
#include "../Object.h"
#include "../Player.h"
#include "../Struct/Coordinate.h"
#include "../Define.h"

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

void UMoveComponent::OpenShop()
{
	Vector Vec = PlayerPtr->GetPosition();
	const Vector checkPositions[] = {
		{ Vec.X, Vec.Y - 1 },
		{ Vec.X, Vec.Y + 1 },
		{ Vec.X - 1, Vec.Y },
		{ Vec.X + 1, Vec.Y }
	};

	bool bNearShop = false;
	for (const Vector& checkPosition : checkPositions)
	{
		if (checkPosition.X < 0 || checkPosition.X >= MAP_MAX_X || checkPosition.Y < 0 || checkPosition.Y >= MAP_MAX_Y)
		{
			continue;
		}

		if (MapManager::GetInstance()->GetType(checkPosition) == MapObjectType::Shop)
		{
			bNearShop = true;
			break;
		}
	}

	if (!bNearShop)
	{
		bWasOnShop = false;
		return;
	}

	if (!bWasOnShop)
	{
		if (UInventoryComponent* InventoryComponent = PlayerPtr->GetComponent<UInventoryComponent>())
		{
			ShopManager::GetInstance()->SetPlayerInventory(InventoryComponent);
			InventoryComponent->OpenShop(1);
		}
	}

	bWasOnShop = true;
}

void UMoveComponent::HandleMoveInput()
{
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::UP))
	{
		int NextX = PlayerPtr->GetPosition().X;
		int NextY = max(PlayerPtr->GetPosition().Y - 1, 0);

		if (MapManager::GetInstance()->IsTypeExist(NextX, NextY, MapObjectType::Monster))
		{
			SetFacingDirection(EDirection::UP);
			MoveElapsedTime = 0.0f;
			return;
		}

		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ NextX, NextY });
		SetFacingDirection(EDirection::UP);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::DOWN))
	{
		int NextX = PlayerPtr->GetPosition().X;
		int NextY = PlayerPtr->GetPosition().Y + 1;

		if (MapManager::GetInstance()->IsTypeExist(NextX, NextY, MapObjectType::Monster))
		{
			SetFacingDirection(EDirection::DOWN);
			MoveElapsedTime = 0.0f;
			return;
		}

		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ NextX, NextY });
		SetFacingDirection(EDirection::DOWN);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::LEFT))
	{
		int NextX = max(PlayerPtr->GetPosition().X - 1, 0);
		int NextY = PlayerPtr->GetPosition().Y;

		if (MapManager::GetInstance()->IsTypeExist(NextX, NextY, MapObjectType::Monster))
		{
			SetFacingDirection(EDirection::LEFT);
			MoveElapsedTime = 0.0f;
			return;
		}

		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ NextX, NextY });
		SetFacingDirection(EDirection::LEFT);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::RIGHT))
	{
		int NextX = PlayerPtr->GetPosition().X + 1;
		int NextY = PlayerPtr->GetPosition().Y;

		if (MapManager::GetInstance()->IsTypeExist(NextX, NextY, MapObjectType::Monster))
		{
			SetFacingDirection(EDirection::RIGHT);
			MoveElapsedTime = 0.0f;
			return;
		}

		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ NextX, NextY });
		SetFacingDirection(EDirection::RIGHT);
		MoveElapsedTime = 0.0f;
	}
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
	
	OpenShop();
	if (UInventoryComponent* InventoryComponent = PlayerPtr->GetComponent<UInventoryComponent>())
	{
		if (InventoryComponent->GetOpenedInventory())
		{
			return;
		}
	}
	HandleMoveInput();
}

/*void UMoveComponent::Tick(float DeltaTime)
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
	
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::UP))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, max(PlayerPtr->GetPosition().Y - 1, 0) });
		SetFacingDirection(EDirection::UP);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::DOWN))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
		SetFacingDirection(EDirection::DOWN);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::LEFT))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ max(PlayerPtr->GetPosition().X - 1, 0 ), PlayerPtr->GetPosition().Y});
		SetFacingDirection(EDirection::LEFT);
		MoveElapsedTime = 0.0f;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::RIGHT))
	{
		PlayerPtr->SetPrevPosition(PlayerPtr->GetPosition());
		PlayerPtr->SetPosition({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y});
		SetFacingDirection(EDirection::RIGHT);
		MoveElapsedTime = 0.0f;
	}
}*/
