#pragma once
#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Enum/Direction.h"
#include "../Item/AllItems.h"
#include "../Struct/Vector.h"

class Player;

class UMoveComponent : public UComponent
{
	friend class AObject;

public:
	static constexpr ComponentType Type = ComponentType::MovementComponent;

protected:
	UMoveComponent(AObject* InOwner);
	UMoveComponent() = delete;

public:
	~UMoveComponent();

	EDirection GetFacingDirection() { return FacingDirection; }
	EDirection GetPreviousFacingDirection() { return PreviousFacingDirection; }
	
	void SetFacingDirection(EDirection Str);
	float GetMoveAlpha() const;
	float GetTurnAlpha() const;
	float GetTeleportCooldownAlpha() const;
	float GetTeleportRemainingTime() const;
	
public:
	void OpenShop();
	void HandleMoveInput();

public:
	virtual void Tick(float DeltaTime) override;

private:
	Player* PlayerPtr = nullptr;
	EDirection FacingDirection;
	EDirection PreviousFacingDirection;
	float MoveElapsedTime = 0.0f;
	float MoveInterval = 0.12f;
	float TurnElapsedTime = 0.0f;
	float TurnDuration = 0.18f;
	bool bWasOnShop = false;
	
	
public:
	void Teleport();
	bool CanTeleportPosition(Vector Position);
	bool FindTeleportPosition(Vector TargetPosition, Vector& OutPosition);
	Vector GetTeleportTargetPosition();
	
	bool IsNearShop(Vector Position);
	
private:
	float TeleportElapsedTime = 999.0f;
	float TeleportInterval = 3.0f;
	int TeleportDistance = 4;
	
	Vector NextPositions[4];
	Vector TeleportDirections[4];
};

