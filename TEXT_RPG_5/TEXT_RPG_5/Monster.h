#pragma once

#include "pch.h"
#include "Vector.h"

class Monster
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
};
