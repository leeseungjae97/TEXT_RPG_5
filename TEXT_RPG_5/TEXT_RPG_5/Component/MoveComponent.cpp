#include "MoveComponent.h"
#include "../Manager/RenderManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/MapManager.h"
#include "../Manager/SceneManager.h"
#include "../Object.h"
#include "../Player.h"
#include  "../Monster.h"
#include "../Struct/Coordinate.h"
#include "InventoryComponent.h"

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
	
	// 인벤 토리가 열려있는지 확인
	if (UInventoryComponent* Inventory = PlayerPtr->GetComponent<UInventoryComponent>())
	{
		if (Inventory->GetOpenedInventory())
		{
			return;
		}
	}

	MoveElapsedTime += DeltaTime;
	TurnElapsedTime += DeltaTime;

	if (MoveElapsedTime < MoveInterval)
	{
		return;
	}

	// 다음 위치에 몬스터가 있는지 확인
	auto IsMonsterAtPosition = [](int X, int Y) -> bool
	{
		vector<AObject*>& Objects = SceneManager::GetInstance()->GetObjects();

		for (AObject* Obj : Objects)
		{
			if (Obj == nullptr || Obj->IsDestroy())
			{
				continue;
			}

			Monster* Mon = dynamic_cast<Monster*>(Obj);

			if (Mon == nullptr)
			{
				continue;
			}

			Vector MonPosition = Mon->GetPosition();

			if (MonPosition.X == X && MonPosition.Y == Y)
			{
				return true;
			}
		}

		return false;
	};
	
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::UP))
	{
		int NextX = PlayerPtr->GetPosition().X;
		int NextY = max(PlayerPtr->GetPosition().Y - 1, 0);

		if (IsMonsterAtPosition(NextX, NextY))
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

		if (IsMonsterAtPosition(NextX, NextY))
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

		if (IsMonsterAtPosition(NextX, NextY))
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

		if (IsMonsterAtPosition(NextX, NextY))
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

bool UMoveComponent::IsMonsterAtPosition(int X, int Y)
{
	vector<AObject*>& Objects = SceneManager::GetInstance()->GetObjects();

	for (AObject* Obj : Objects)
	{
		if (Obj == nullptr || Obj->IsDestroy())
		{
			continue;
		}

		if (Monster* Mon = dynamic_cast<Monster*>(Obj))
		{
			Vector MonPos = Mon->GetPosition();

			if (MonPos.X == X && MonPos.Y == Y)
			{
				return true;
			}
		}
	}

	return false;
}

// 몬스터 박치기 x 버전
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
