#include "Player.h"
#include "InputManager.h"
#include "MoveComponent.h"
#include "RenderManager.h"

// ������
Player::Player(string str, int hp, int power) : Name(str), HP(hp), Power(power)
{
	this->Level = 1;
	this->Max_HP = HP;
	this->Exp = 0;
	this->Max_Exp = 100;
	this->Gold = 0;
	this->MoveComponent = CreateDefaultComponent<UMoveComponent>();

	this->Position.Y = 2;
	this->Position.X = 2;
	//this->FacingDirection = EDirection::None;
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
	return Name;
}
int Player::GetLevel()
{
	return Level;
}
int Player::GetPower()
{
	return Power;
}
int Player::GetHP()
{
	return HP;
}
int Player::GetMax_HP()
{
	return Max_HP;
}
int Player::GetExp()
{
	return Exp;
}
int Player::GetMax_Exp()
{
	return Max_Exp;
}
int Player::GetGold()
{
	return Gold;
}

// Setter
void Player::SetName(string Name)
{
	this->Name = Name;
}
void Player::SetLevel(int Level)
{
	this->Level = Level;
}
void Player::SetPower(int Power)
{
	this->Power = Power;
}
void Player::SetHP(int HP)
{
	this->HP = HP;
}
void Player::SetMax_HP(int Max_HP)
{
	this->Max_HP = Max_HP;
}
void Player::SetExp(int Exp)
{
	this->Exp = Exp;
}
void Player::SetMax_Exp(int Max_Exp)
{
	this->Max_Exp = Max_Exp;
}
void Player::SetGold(int Gold)
{
	this->Gold = Gold;
}