#pragma once

#include "pch.h"
#include "Struct/Vector.h"
#include "Object.h"
#include "struct/ItemWeight.h"

class Player;

class Monster : public AObject
{
protected:
	wstring Name;
	int Level;
	int Health;
	int MaxHealth;
	int Attack;

protected:
	
	float MoveElapsedtime;
	
	float MoveInterval;
	
	int DetectionRange;
	
	bool bUseBfs;
	
	int AttackRange;
	float AttackElapsedtime;
	float AttackInterval;
	float AttackVisibleTime;
	float AttackVisibleDuration;
	float AttackTelegraphElapsedTime;
	float AttackTelegraphDuration;
	bool bAttackTelegraphActive;
	vector<Vector> AttackValue;

	bool bIsShiny = false;
	bool bIsBoss = false;
	
public:
	Monster();
	virtual ~Monster();

	string GetName();
	const wstring& GetDisplayName() const { return Name; }
	int GetLevel() const { return Level; }
	int GetHealth();
	int GetMaxHealth();
	int GetAttack();
	float GetMoveAlpha() const;
	bool IsAttackVisible() const { return AttackVisibleTime > 0.0f; }
	bool IsAttackTelegraphActive() const { return bAttackTelegraphActive; }
	const vector<Vector>& GetAttackValue() const { return AttackValue; }

	void TakeDamage(int damage);
	bool IsDead();
	void TransitionAttackTelegraphToAttack(float DeltaTime);

	bool IsShiny() const;
	void TrySetShiny();
	bool IsBoss() const { return bIsBoss; }
	void SetBoss(bool InBoss) { bIsBoss = InBoss; }
	void ConfigureForStage(int InLevel, bool InBoss = false);
	

	

public:
	virtual void BeginPlay()	override{}
	virtual void Tick(float DeltaTime) override;
	virtual void Destroy() override;
	virtual void OnSpawnFromPool() override;
	virtual void OnReturnToPool() override;

protected:
	Player* FindPlayer();

	
	int GetDistanceToPlayer(Player* player);
	
	bool CanAttackplayer(Player* player);
	
	void Attackplayer(Player* player);
	virtual void BuildAttackValue(Player* player);
	void ExecutePendingAttack(Player* player);
	
	void MoveTowardPlayer(Player* player);
	
	void MoveTowardPlayerBfs();
	
	void MoveRandom();
	
	virtual vector<FItemWeight> GetDropTable();
	void DropItemToPlayer();
	
};
