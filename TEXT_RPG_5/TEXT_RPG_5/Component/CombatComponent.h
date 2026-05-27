#pragma once
#include "Component.h"
#include "../pch.h"
#include "../Enum/ComponentTypeEnum.h"
#include "../Struct/Vector.h"
#include "../Enum/WeaponType.h"

class Player;
class UMoveComponent;

struct FTeleportEffect
{
	Vector Position;
	float Elapsed = 0.0f;
	float Duration = 0.6f;
};

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
	float GetAttackAnimationAlpha() const;
	// void SetAttackValue();
	void Attack();
	void TriggerCustomAttack(const vector<Vector>& CustomRange, int Damage);
	void HandleAttack();
	void LaunchProjectile();
	void MakeSwordRange();
	void MakeAxeRange();
	void MakeRockRange();
	void MakeMagicRange();
	void AttackEffectTimeAcc(float DeltaTime);
	void HandleAttackInput(float DeltaTime);
	void PlayTeleportEffect(Vector StartPosition, Vector EndPosition, float Duration = 0.6f);
	const vector<FTeleportEffect>& GetTeleportEffects() const { return TeleportEffects; }
	
	void ProjectileAttack();
	
	WeaponType GetWeaponType() { return Weapon;}
	void SetWeaponType(WeaponType InWeaponType) { Weapon = InWeaponType;}
	
public:
	virtual void Tick(float DeltaTime) override;

private:
	vector<Vector> AttackValue;
	Player* PlayerPtr;
	UMoveComponent* MoveComponentPtr;
	WeaponType Weapon;
	float AttackVisibleTime;
	float AttackVisibleDuration;
	bool bAttackRequested;
	
	
	float RockInterval;
	float SwordInterval;
	float AxeInterval;
	float BowInterval;
	float MagicInterval;

	float RockElapsedTime;
	float SwordElapsedTime;
	float AxeElapsedTime;
	float BowElapsedTime;
	float MagicElapsedTime;
	vector<FTeleportEffect> TeleportEffects;
};
