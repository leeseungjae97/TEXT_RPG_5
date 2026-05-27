#pragma once

#include "pch.h"
#include "Struct/Vector.h"
#include "Object.h"

class Player;

class Monster : public AObject
{
protected:
	string Name;
	int Health;
	int MaxHealth;
	int Attack;

protected:
	
	float MoveElapsedtime;
	
	float MoveInterval;
	
	int DetectionRange;
	
	bool bUseBfs;
	
	int AttackRange;
	float AttackElapsedtime;
	float AttackInterval;
	float AttackVisibleTime;
	float AttackVisibleDuration;
	vector<Vector> AttackValue;

	
public:
	Monster();
	virtual ~Monster();

	string GetName();
	int GetHealth();
	int GetMaxHealth();
	int GetAttack();
	float GetMoveAlpha() const;
	bool IsAttackVisible() const { return AttackVisibleTime > 0.0f; }
	const vector<Vector>& GetAttackValue() const { return AttackValue; }

	void TakeDamage(int damage);
	bool IsDead();

	

public:
	virtual void BeginPlay()	override{}
	virtual void Tick(float DeltaTime) override;
	virtual void Destroy() override;
	virtual void OnSpawnFromPool() override;
	virtual void OnReturnToPool() override;

protected:
	Player* FindPlayer();

	
	int GetDistanceToPlayer(Player* player);
	
	bool CanAttackplayer(Player* player);
	
	void Attackplayer(Player* player);
	
	void MoveTowardPlayer(Player* player);
	
	void MoveTowardPlayerBfs();
	
	void MoveRandom();
};
