#include "Player.h"

// 생성자
Player::Player(string str, int hp, int power) : Name(str), HP(hp), Power(power)
{
	this->Level = 1;
	this->Max_HP = HP;
	this->Exp = 0;
	this->Max_Exp = 100;
	this->Gold = 0;
}

// 상태창 호출 시
void Player::DisplayStatus()
{
	cout << "============================\n";
	cout << " " << Name << " 의 현재 능력치\n";
	cout << "============================\n";
	cout << "Lv. " << Level << "    Exp : (" << Exp << " / " << Max_Exp << ")\n";
	cout << "HP : " << HP << "    " << "공격력 : " << Power << "\n";
	cout << "============================\n";
}

// 아이템 사용
//void Player::UseItem(int index)
//{
//	cout << "아이템을 사용합니다.\n";
//	//invento
//}

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
void Player::GetName(string Name)
{
	this->Name = Name;
}
void Player::GetLevel(int Level)
{
	this->Level = Level;
}
void Player::GetPower(int Power)
{
	this->Power = Power;
}
void Player::GetHP(int HP)
{
	this->HP = HP;
}
void Player::GetMax_HP(int Max_HP)
{
	this->Max_HP = Max_HP;
}
void Player::GetExp(int Exp)
{
	this->Exp = Exp;
}
void Player::GetMax_Exp(int Max_Exp)
{
	this->Max_Exp = Max_Exp;
}
void Player::GetGold(int Gold)
{
	this->Gold = Gold;
}