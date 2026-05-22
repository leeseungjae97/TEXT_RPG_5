#include "Player.h"
#include "InputManager.h"

// ������
Player::Player(string str, int hp, int power) : Name(str), HP(hp), Power(power)
{
	this->Level = 1;
	this->Max_HP = HP;
	this->Exp = 0;
	this->Max_Exp = 100;
	this->Gold = 0;
	Position = Vector(2, 2);
	PrevPosition = Vector(2, 2);
}

Player::~Player()
{
}

// ����â ȣ�� ��
void Player::DisplayStatus()
{
	cout << "============================\n";
	cout << " " << Name << " �� ���� �ɷ�ġ\n";
	cout << "============================\n";
	cout << "Lv. " << Level << "    Exp : (" << Exp << " / " << Max_Exp << ")\n";
	cout << "HP : " << HP << "    " << "���ݷ� : " << Power << "\n";
	cout << "============================\n";

}
void Player::Tick(float DeltaTime)
{
	//3�ʵڿ� �������� �Դ´�.
	// Elapsed�� ���������
	float Elapsed = 0.0f;
	Elapsed += DeltaTime;
	if (Elapsed >= 3.f)
	{

	}

	PrevPosition = Position;
	//if (InputManager::GetInstance()->IsKeyDown(eKeyCode::UP))
	//{
	//	Position.Y--;
	//	Sleep(120);
	//}
	//if (InputManager::GetInstance()->IsKeyDown(eKeyCode::DOWN))
	//{
	//	Position.Y++;
	//	Sleep(120);
	//}
	//if (InputManager::GetInstance()->IsKeyDown(eKeyCode::RIGHT))
	//{
	//	Position.X++;
	//	Sleep(120);
	//}
	//if (InputManager::GetInstance()->IsKeyDown(eKeyCode::LEFT))
	//{
	//	Position.X--;
	//	Sleep(120);
	//}
}

// ������ ���
//void Player::UseItem(int index)
//{
//	cout << "�������� ����մϴ�.\n";
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