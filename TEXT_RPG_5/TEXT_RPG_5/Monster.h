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

protected:
	Player* FindPlayer();

	
	int GetDistanceToPlayer(Player* player);


	void MoveTowardPlayer(Player* player);


	void MoveRandom();
};
