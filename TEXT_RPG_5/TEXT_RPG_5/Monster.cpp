#include "Monster.h"

Monster::Monster()
{
	Name = "Monster";
	Health = 100;
	Attack = 10;

}

string Monster::GetName()
{
	return Name;
}

int Monster::GetHealth()
{
	return Health;
}

int Monster::GetAttack()
{
	return Attack;
}

void Monster::TakeDamage(int Damage)
{
	Health -= Damage;

	if (Health < 0)
	{
		Health = 0;
	}
}

bool Monster::IsDead()
{
	return Health <= 0;
}


