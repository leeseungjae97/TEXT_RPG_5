#pragma once
#include "Vector.h"
#include "pch.h"
#include "Component.h"

class Player;
class UMoveComponent;

class UCombatComponent : public UComponent
{
public:
	UCombatComponent(AObject* InOwner);
	~UCombatComponent();

	vector<Vector> GetAttackValue();
	// void SetAttackValue();

public:
	virtual void Tick(float DeltaTime) override;

private:
	vector<Vector> AttackValue;
	Player* PlayerPtr;
	UMoveComponent* MoveComponentPtr;
	float TotalTime;
	float DelayTime;
};
