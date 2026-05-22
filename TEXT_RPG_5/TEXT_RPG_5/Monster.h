#pragma once

#include "pch.h"
#include "Vector.h"
#include "Object.h"

class Monster : public AObject
{
protected:
	string Name;
	int Health;
	int Attack;

public:
	Monster();
	string GetName();
	int GetHealth();
	int GetAttack();
	void TakeDamage(int damage);
	bool IsDead();

	void Update(float DeltaTime);

public:
	virtual void Init() {}
	virtual void Tick(float DeltaTime){}
	virtual void Destroy(){}
};
