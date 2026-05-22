#pragma once
#include "Component.h"

enum class EDirection
{
	LeftUp = 1,
	Up = 2,
	RightUp = 3,
	Left = 4,
	None = 5,
	Right = 6,
	LeftDown = 7,
	Down = 8,
	RightDown = 9
};

class UMoveComponent : public UComponent
{
public:
	UMoveComponent(AObject* InOwner);
	~UMoveComponent();

	void SetFacingDirection(EDirection Str);
	EDirection GetFacingDirection();

public:
	virtual void Tick(float DeltaTime) override;


private:
	EDirection FacingDirection;
};

