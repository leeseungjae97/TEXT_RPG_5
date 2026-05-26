#pragma once
#include "Component.h"
#include "../pch.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Struct/Vector.h"
#include "../Enum/WeaponType.h"

class Player;
class UMoveComponent;

class UCombatComponent : public UComponent
{
	friend class AObject;

public:
	static constexpr ComponentType Type = ComponentType::CombatComponent;
	
protected:
	UCombatComponent(AObject* InOwner);
	UCombatComponent() = delete;

public:
	~UCombatComponent();

	vector<Vector> GetAttackValue();
	bool IsAttackVisible() const { return AttackVisibleTime > 0.0f; }
	bool IsAttackCoolingDown() const;
	float GetAttackCooldownAlpha() const;
	// void SetAttackValue();
	void SwordAttack();
	void HandleAttack();
	void HandleAttackInput(float DeltaTime);
	
	WeaponType GetWeaponType() { return Weapon;}
	void SetWeaponType(WeaponType InWeaponType) { Weapon = InWeaponType;}
	
public:
	virtual void Tick(float DeltaTime) override;

private:
	vector<Vector> AttackValue;
	Player* PlayerPtr;
	UMoveComponent* MoveComponentPtr;
	WeaponType Weapon;
	float AttackElapsedTime;
	float AttackInterval;
	float AttackVisibleTime;
	float AttackVisibleDuration;
	bool bAttackRequested;
	
};
