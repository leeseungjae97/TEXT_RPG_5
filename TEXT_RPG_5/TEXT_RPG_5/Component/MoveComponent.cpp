#include "MoveComponent.h"

#include "InventoryComponent.h"
#include "CombatComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/MapManager.h"
#include "../Manager/ShopManager.h"
#include "../Object.h"
#include "../Player.h"
#include "../Struct/Coordinate.h"
#include "../Define.h"
#include "../pch.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/ViewportManager.h"
#include "EquipmentComponent.h"

namespace
{
	EDirection TurnLeft(EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::UP:
			return EDirection::LEFT;
		case EDirection::LEFT:
			return EDirection::DOWN;
		case EDirection::DOWN:
			return EDirection::RIGHT;
		case EDirection::RIGHT:
			return EDirection::UP;
		case EDirection::NONE:
		default:
			return EDirection::UP;
		}
	}

	EDirection TurnRight(EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::UP:
			return EDirection::RIGHT;
		case EDirection::RIGHT:
			return EDirection::DOWN;
		case EDirection::DOWN:
			return EDirection::LEFT;
		case EDirection::LEFT:
			return EDirection::UP;
		case EDirection::NONE:
		default:
			return EDirection::UP;
		}
	}

	EDirection TurnBack(EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::UP:
			return EDirection::DOWN;
		case EDirection::DOWN:
			return EDirection::UP;
		case EDirection::LEFT:
			return EDirection::RIGHT;
		case EDirection::RIGHT:
			return EDirection::LEFT;
		case EDirection::NONE:
		default:
			return EDirection::DOWN;
		}
	}

	Vector GetDirectionOffset(EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::UP:
			return { 0, -1 };
		case EDirection::DOWN:
			return { 0, 1 };
		case EDirection::LEFT:
			return { -1, 0 };
		case EDirection::RIGHT:
			return { 1, 0 };
		case EDirection::NONE:
		default:
			return { 0, 0 };
		}
	}
}

UMoveComponent::UMoveComponent(AObject* InOwner)
	: UComponent(InOwner)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	this->FacingDirection = EDirection::UP;
	this->PreviousFacingDirection = EDirection::UP;
	
	TeleportDirections[0] = { 0, -1 };
	TeleportDirections[1] = { 0, 1 };  
	TeleportDirections[2] = { -1, 0 }; 
	TeleportDirections[3] = { 1, 0 };
	
	NextPositions[0] = { 0, 0 };
	NextPositions[1] = { 0, 0 };
	NextPositions[2] = { 0, 0 };
	NextPositions[3] = { 0, 0 };
}

UMoveComponent::~UMoveComponent()
{

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

float UMoveComponent::GetTeleportCooldownAlpha() const
{
	if (TeleportInterval <= 0.0f)
	{
		return 1.0f;
	}

	return min(max(TeleportElapsedTime / TeleportInterval, 0.0f), 1.0f);
}

float UMoveComponent::GetTeleportRemainingTime() const
{
	return max(0.0f, TeleportInterval - TeleportElapsedTime);
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
		ViewportManager::GetInstance()->OpenEnterShop();
	}

	bWasOnShop = true;
}

void UMoveComponent::HandleMoveInput()
{
	if (PlayerPtr->IsMoving())
	{
		return;
	}

	if (ViewportManager::GetInstance()->Is3DMode())
	{
		if (InputManager::GetInstance()->IsKeyTap(KeyCode::LEFT))
		{
			SetFacingDirection(TurnRight(FacingDirection));
			MoveElapsedTime = 0.0f;
			return;
		}

		if (InputManager::GetInstance()->IsKeyTap(KeyCode::RIGHT))
		{
			SetFacingDirection(TurnLeft(FacingDirection));
			MoveElapsedTime = 0.0f;
			return;
		}

		if (InputManager::GetInstance()->IsKeyTap(KeyCode::DOWN))
		{
			SetFacingDirection(TurnBack(FacingDirection));
			MoveElapsedTime = 0.0f;
			return;
		}

		if (!InputManager::GetInstance()->IsKeyPressed(KeyCode::UP))
		{
			return;
		}

		Vector Offset = GetDirectionOffset(FacingDirection);
		if (Offset.X == 0 && Offset.Y == 0)
		{
			return;
		}

		Vector NextPosition = {
			PlayerPtr->GetPosition().X + Offset.X,
			PlayerPtr->GetPosition().Y + Offset.Y
		};

		if (NextPosition.X < 1 || NextPosition.X >= MAP_MAX_X - 1 ||
			NextPosition.Y < 1 || NextPosition.Y >= MAP_MAX_Y - 1)
		{
			return;
		}

		if (!MapManager::GetInstance()->CanMoveObjectTo(PlayerPtr, NextPosition))
		{
			return;
		}

		PlayerPtr->BeginMoveTo(NextPosition);
		MoveElapsedTime = 0.0f;
		return;
	}

	int NextX = PlayerPtr->GetPosition().X;
	int NextY = PlayerPtr->GetPosition().Y;
	EDirection NextDirection = EDirection::NONE;
	
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::UP))
	{
		NextY = max(NextY - 1, 0);
		NextDirection = EDirection::UP;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::DOWN))
	{
		NextY = min(NextY + 1, MAP_MAX_Y);
		NextDirection = EDirection::DOWN;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::LEFT))
	{
		NextX = max(NextX - 1, 0);
		NextDirection = EDirection::LEFT;
	}
	else if (InputManager::GetInstance()->IsKeyPressed(KeyCode::RIGHT))
	{
		NextX = min(NextX + 1, MAP_MAX_X);
		NextDirection = EDirection::RIGHT;
	}

	if (NextDirection == EDirection::NONE)
	{
		return;
	}

	Vector NextPosition = {NextX, NextY};
	if (NextPosition.X < 1 || NextPosition.X >= MAP_MAX_X - 1 ||
		NextPosition.Y < 1 || NextPosition.Y >= MAP_MAX_Y - 1)
	{
		SetFacingDirection(NextDirection);
		return;
	}

	if (!MapManager::GetInstance()->CanMoveObjectTo(PlayerPtr, NextPosition))
	{
		SetFacingDirection(NextDirection);
		return;
	}

	SetFacingDirection(NextDirection);
	PlayerPtr->BeginMoveTo(NextPosition);
	MoveElapsedTime = 0.0f;
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
	if (UCombatComponent* CombatComponent = PlayerPtr->GetComponent<UCombatComponent>())
	{
		CombatComponent->PlayTeleportEffect(CurrentPosition, FinalPosition);
	}
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
	if (!MapManager::GetInstance()->CanMoveObjectTo(PlayerPtr, Position))
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

	const int MaxTeleportRange = 4;
	const int LocalSize = MaxTeleportRange * 2 + 1;

	int Range = TeleportDistance;
	if (Range > MaxTeleportRange)
	{
		Range = MaxTeleportRange;
	}

	int VisitedRange = Range * 2 + 1;

	int LocalMapX = PlayerPosition.X - Range;
	int LocalMapY = PlayerPosition.Y - Range;

	bool Visited[LocalSize][LocalSize] = {};

	CheckList.push_back(TargetPosition);

	int TargetLocalX = TargetPosition.X - LocalMapX;
	int TargetLocalY = TargetPosition.Y - LocalMapY;

	if (TargetLocalX >= 0 && TargetLocalX < VisitedRange &&
		TargetLocalY >= 0 && TargetLocalY < VisitedRange)
	{
		Visited[TargetLocalY][TargetLocalX] = true;
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

		for (int i = 0; i < 4; ++i)
		{
			NextPositions[i] = {
				Current.X + TeleportDirections[i].X,
				Current.Y + TeleportDirections[i].Y
			};
		}
		
		for (int i = 3; i > 0; --i)
		{
			int RandomIndex = rand() % (i + 1);

			Vector Temp = NextPositions[i];
			NextPositions[i] = NextPositions[RandomIndex];
			NextPositions[RandomIndex] = Temp;
		}

		for (int i = 0; i < 4; ++i)
		{
			Vector Next = NextPositions[i];

			if (Next.X < 0 || Next.X >= MAP_MAX_X ||
				Next.Y < 0 || Next.Y >= MAP_MAX_Y)
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

			int LocalX = Next.X - LocalMapX;
			int LocalY = Next.Y - LocalMapY;

			if (LocalX < 0 || LocalX >= VisitedRange ||
				LocalY < 0 || LocalY >= VisitedRange)
			{
				continue;
			}

			if (Visited[LocalY][LocalX])
			{
				continue;
			}

			Visited[LocalY][LocalX] = true;
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

	if (PlayerPtr->IsMoving())
	{
		PlayerPtr->CommitMoveIfNeeded(GetMoveAlpha());
	}

	if (MoveElapsedTime < MoveInterval)
	{
		return;
	}

	PlayerPtr->FinishMoveIfNeeded(GetMoveAlpha());
	
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
		UEquipmentComponent* EquipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>();

		if (EquipmentComponent == nullptr)
		{
			return;
		}

		WeaponType CurrentWeapon = EquipmentComponent->GetCurrentWeaponType();
		
		// 매직 웨폰 타입 확인
		if (CurrentWeapon != WeaponType::Magic)
		{
			return;
		}
		
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
