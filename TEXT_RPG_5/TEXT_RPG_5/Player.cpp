#include "Player.h"
#include "./Manager/InputManager.h"
#include "Component/MoveComponent.h"
#include "Component/CombatComponent.h"

Player::Player(string str, int hp, int power) : TotalStat{ str, hp, power }
{
	this->TotalStat.Level = 1;
	this->TotalStat.Max_HP = TotalStat.HP;
	this->TotalStat.Exp = 0;
	this->TotalStat.Max_Exp = 100;
	this->TotalStat.Gold = 0;
	this->TotalStat.IsAttack = false;

	this->MoveComponent = CreateDefaultComponent<UMoveComponent>();
	this->CombatComponent = CreateDefaultComponent<UCombatComponent>();

	this->Position.Y = 2;
	this->Position.X = 2;
	this->PrevPosition = this->Position;

	//Position = Vector(2, 2);
	//PrevPosition = Vector(2, 2);
}

Player::~Player()
{
}

void Player::Tick(float DeltaTime)
{
	AObject::Tick(DeltaTime);
}

// Getter
string Player::GetName()
{
	return this->TotalStat.Name;
}
int Player::GetLevel()
{
	return this->TotalStat.Level;
}
int Player::GetPower()
{
	return this->TotalStat.Power;
}
int Player::GetHP()
{
	return this->TotalStat.HP;
}
int Player::GetMax_HP()
{
	return this->TotalStat.Max_HP;
}
int Player::GetExp()
{
	return this->TotalStat.Exp;
}
int Player::GetMax_Exp()
{
	return this->TotalStat.Max_Exp;
}
int Player::GetGold()
{
	return this->TotalStat.Gold;
}
bool Player::GetIsAttack()
{
	return this->TotalStat.IsAttack;
}

// Setter
void Player::SetName(string Name)
{
	this->TotalStat.Name = Name;
}
void Player::SetLevel(int Level)
{
	this->TotalStat.Level = Level;
}
void Player::SetPower(int Power)
{
	this->TotalStat.Power = Power;
}
void Player::SetHP(int HP)
{
	this->TotalStat.HP = HP;
}
void Player::SetMax_HP(int Max_HP)
{
	this->TotalStat.Max_HP = Max_HP;
}
void Player::SetExp(int Exp)
{
	this->TotalStat.Exp = Exp;
}
void Player::SetMax_Exp(int Max_Exp)
{
	this->TotalStat.Max_Exp = Max_Exp;
}
void Player::SetGold(int Gold)
{
	this->TotalStat.Gold = Gold;
}
void Player::SetIsAttack(bool Value)
{
	this->TotalStat.IsAttack = Value;
}
