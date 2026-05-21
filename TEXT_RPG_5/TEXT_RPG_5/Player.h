#pragma once
#include "pch.h"
//#include "Vector.h"

class Player
{
public:
	// 생성자
	Player(string str, int hp, int power);
	
	// 소멸자
	~Player() {}

	// 상태창 호출
	void DisplayStatus();

	// Getter
	string GetName();
	int GetLevel();
	int GetPower();
	int GetHP();
	int GetMax_HP();
	int GetExp();
	int GetMax_Exp();
	int GetGold();

	// Setter
	void SetName(string Name);
	void SetLevel(int Level);
	void SetPower(int Power);
	void SetHP(int HP);
	void SetMax_HP(int Max_HP);
	void SetExp(int Exp);
	void SetMax_Exp(int Max_Exp);
	void SetGold(int Gold);

protected:
	string Name;
	int Level;
	int HP;
	int Max_HP;
	int Power;
	int Exp;
	int Max_Exp;
	int Gold;
	//Vector Poition
};