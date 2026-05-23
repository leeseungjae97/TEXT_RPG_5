#pragma once
#include "Vector.h"
#include "pch.h"
#include "Component.h"
#include "ComponentTypeEnum.h"

class Player;
class UMoveComponent;

class UCombatComponent : public UComponent
{
	friend class AObject;

public:
	static constexpr ComponentType Type = ComponentType::CombatComponent;

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
