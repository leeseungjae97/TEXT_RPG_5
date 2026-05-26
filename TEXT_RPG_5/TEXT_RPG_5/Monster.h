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

	
public:
	Monster();
	virtual ~Monster();

	string GetName();
	int GetHealth();
	int GetMaxHealth();
	int GetAttack();
	float GetMoveAlpha() const;

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


	void MoveTowardPlayer(Player* player);
	
	
	void MoveTowardPlayerBfs();


	void MoveRandom();
};
