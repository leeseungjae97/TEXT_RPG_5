#pragma once
#include "Component.h"

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
public:
	UMoveComponent(AObject* InOwner);
	~UMoveComponent();

	EDirection GetFacingDirection();
	void SetFacingDirection(EDirection Str);
	

public:
	virtual void Tick(float DeltaTime) override;

private:
	Player* PlayerPtr;
	EDirection FacingDirection;
};

