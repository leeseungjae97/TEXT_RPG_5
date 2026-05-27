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
			InventoryComponent->OpenShop();
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

// 여기서 부터 작업
/*bool UMoveComponent::HandleTeleportInput()
{
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::SHIFT))
		return true;
	return false;
}*/

void UMoveComponent::Teleport()
{
	if (PlayerPtr == nullptr)
	{
		return;
	}

	Vector CurrentPosition = PlayerPtr->GetPosition();
	Vector TargetPosition = GetTeleportTargetPosition();
	Vector FinalPosition = {0, 0};

	// 1. 바로 가는 경우
	if (CanTeleportPosition(TargetPosition))
	{
		FinalPosition = TargetPosition;
	}
	// 2. 막힌 경우
	else
	{
		// 그 근방에 텔레포트 거리에 맞는 위치 찾기
		if (!FindTeleportPosition(TargetPosition, FinalPosition))
		{
			return;
		}
	}
	
	// 이런 느낌
	PlayerPtr->SetPrevPosition(CurrentPosition);
	PlayerPtr->SetPosition(FinalPosition);
	/*// 이런 느낌
	PlayerPtr->SetPrevPosition(FinalPosition);
	PlayerPtr->SetPosition(FinalPosition);*/
}

// 이동 가능 여부 확인
bool UMoveComponent::CanTeleportPosition(Vector Position)
{
	// 맵 밖 X
	if (Position.X < 0 || Position.X >= MAP_MAX_X ||
		Position.Y < 0 || Position.Y >= MAP_MAX_Y)
	{
		return false;
	}
	// 벽 X
	if (MapManager::GetInstance()->IsTypeExist(Position, MapObjectType::Wall))
	{
		return false;
	}
	// 몬스터 X
	if (MapManager::GetInstance()->IsTypeExist(Position, MapObjectType::Monster))
	{
		return false;
	}
	// 상점 X
	if (IsNearShop(Position))
	{
		return false;
	}
	return true; 
}

// 텔레포트 이동 위치 넣어주기
bool UMoveComponent::FindTeleportPosition(Vector TargetPosition, Vector& OutPosition)
{
	if (CanTeleportPosition(TargetPosition))
	{
		OutPosition = TargetPosition;
		return true;
	}

	Vector PlayerPosition = PlayerPtr->GetPosition();

	vector<Vector> CheckList;
	bool Visited[MAP_MAX_Y][MAP_MAX_X] = {};

	CheckList.push_back(TargetPosition);

	if (TargetPosition.X >= 0 && TargetPosition.X < MAP_MAX_X &&
		TargetPosition.Y >= 0 && TargetPosition.Y < MAP_MAX_Y)
	{
		Visited[TargetPosition.Y][TargetPosition.X] = true;
	}

	while (!CheckList.empty())
	{
		Vector Current = CheckList[0];
		CheckList.erase(CheckList.begin());

		int Distance =
			abs(PlayerPosition.X - Current.X) +
			abs(PlayerPosition.Y - Current.Y);

		if (Distance == TeleportDistance && CanTeleportPosition(Current))
		{
			OutPosition = Current;
			return true;
		}

		Vector Up = { Current.X, Current.Y - 1 };
		Vector Down = { Current.X, Current.Y + 1 };
		Vector Left = { Current.X - 1, Current.Y };
		Vector Right = { Current.X + 1, Current.Y };

		Vector NextPositions[4] = { Up, Down, Left, Right };

		for (int i = 0; i < 4; ++i)
		{
			Vector Next = NextPositions[i];
			
			// 맵 밖 제외
			if (Next.X < 0 || Next.X >= MAP_MAX_X ||
				Next.Y < 0 || Next.Y >= MAP_MAX_Y)
			{
				continue;
			}
			
			// 담겨 있는 거면 제외
			if (Visited[Next.Y][Next.X])
			{
				continue;
			}

			int NextDistance =
				abs(PlayerPosition.X - Next.X) +
				abs(PlayerPosition.Y - Next.Y);

			if (NextDistance > TeleportDistance)
			{
				continue;
			}

			Visited[Next.Y][Next.X] = true;
			CheckList.push_back(Next);
		}
	}

	return false;
}

Vector UMoveComponent::GetTeleportTargetPosition()
{
	Vector TargetPosition = PlayerPtr->GetPosition();

	if (FacingDirection == EDirection::UP)
	{
		TargetPosition.Y -= TeleportDistance;
	}
	else if (FacingDirection == EDirection::DOWN)
	{
		TargetPosition.Y += TeleportDistance;
	}
	else if (FacingDirection == EDirection::LEFT)
	{
		TargetPosition.X -= TeleportDistance;
	}
	else if (FacingDirection == EDirection::RIGHT)
	{
		TargetPosition.X += TeleportDistance;
	}

	return TargetPosition;
}

bool UMoveComponent::IsNearShop(Vector Position)
{
	for (int Y = Position.Y - 1; Y <= Position.Y + 1; ++Y)
	{
		for (int X = Position.X - 1; X <= Position.X + 1; ++X)
		{
			if (X < 0 || X >= MAP_MAX_X ||
				Y < 0 || Y >= MAP_MAX_Y)
			{
				continue;
			}

			if (MapManager::GetInstance()->IsTypeExist({ X, Y }, MapObjectType::Shop))
			{
				return true;
			}
		}
	}

	return false;
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
	TeleportElapsedTime += DeltaTime;

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
	
	if (InputManager::GetInstance()->IsKeyTap(KeyCode::C))
	{
		if (TeleportElapsedTime >= TeleportInterval)
		{
			Teleport();
			TeleportElapsedTime = 0.0f;
			MoveElapsedTime = 0.0f;
		}

		return;
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
