#pragma once
#include "Component.h"
#include "ComponentTypeEnum.h"

enum class EDirection : int
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

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
	void SetFacingDirection(EDirection Str);
	

public:
	virtual void Tick(float DeltaTime) override;

private:
	Player* PlayerPtr = nullptr;
	EDirection FacingDirection;
	float MoveElapsedTime = 0.0f;
	float MoveInterval = 0.12f;
};

