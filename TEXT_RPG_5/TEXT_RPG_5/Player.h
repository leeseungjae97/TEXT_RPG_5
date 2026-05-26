#pragma once
#include "pch.h"
#include "Struct/Vector.h"
#include "PlayerStat.h"
#include "Object.h"

class UMoveComponent;
class UCombatComponent;
class UInventoryComponent;
class USubActionComponent;
class Player : public AObject
{
public:
	// 생성자
	Player(string str, int hp, int power);
	
	// 소멸자
	~Player();

	// Getter
	string GetName();
	int GetLevel();
	int GetPower();
	int GetHP();
	int GetMax_HP();
	int GetExp();
	int GetMax_Exp();
	int GetGold();
	bool GetIsAttack();

	// Setter
	void SetName(string Name);
	void SetLevel(int Level);
	void SetPower(int Power);
	void SetHP(int HP);
	void SetMax_HP(int Max_HP);
	void SetExp(int Exp);
	void SetMax_Exp(int Max_Exp);
	void SetGold(int Gold);
	void SetIsAttack(bool Value);

public:
	virtual void BeginPlay(){}
	virtual void Tick(float DeltaTime);
	virtual void Destroy(){}

protected:
	PlayerStat TotalStat;

private:
	UMoveComponent* MoveComponent = nullptr;
	UCombatComponent* CombatComponent = nullptr;
	UInventoryComponent* InventoryComponent = nullptr;
	USubActionComponent* SubActionComponent = nullptr;
};
