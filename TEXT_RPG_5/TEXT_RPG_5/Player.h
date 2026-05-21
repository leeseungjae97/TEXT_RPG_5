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
	void GetName(string Name);
	void GetLevel(int Level);
	void GetPower(int Power);
	void GetHP(int HP);
	void GetMax_HP(int Max_HP);
	void GetExp(int Exp);
	void GetMax_Exp(int Max_Exp);
	void GetGold(int Gold);
	

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