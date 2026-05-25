#pragma once
#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Enum/Direction.h"

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

	EDirection GetFacingDirection();
	EDirection GetPreviousFacingDirection();
	void SetFacingDirection(EDirection Str);
	float GetMoveAlpha() const;
	float GetTurnAlpha() const;
	
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
};

