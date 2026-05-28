#pragma once

#include "pch.h"
#include "Struct/Vector.h"
#include "Object.h"
#include "struct/ItemWeight.h"

class Player;

enum class EMonsterAttackTileType
{
	Default,
	Fire,
	Breath,
	Dark
};

class Monster : public AObject
{
protected:
	wstring Name;
	int Level;
	int Health;
	int MaxHealth;
	int Attack;
	
	int RenderScale = 1;
	//타일테스트
	EMonsterAttackTileType AttackTileType = EMonsterAttackTileType::Default;


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
	int GetRenderScale() const { return RenderScale > 0 ? RenderScale : 1; }
	float GetMoveAlpha() const;
	bool IsAttackVisible() const { return AttackVisibleTime > 0.0f; }
	bool IsAttackTelegraphActive() const { return bAttackTelegraphActive; }
	const vector<Vector>& GetAttackValue() const { return AttackValue; }
	
	//타일테스트
	EMonsterAttackTileType GetAttackTileType() const { return AttackTileType; }

	void TakeDamage(int damage);
	bool IsDead();
	void TransitionAttackTelegraphToAttack(float DeltaTime);

	
	//부활테스트
	virtual bool Revive() { return false; }
	
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
	
	int GetRandomAttackDamage() const;
	
	void MoveTowardPlayer(Player* player);
	
	void MoveTowardPlayerBfs();
	
	void MoveRandom();
	
	virtual vector<FItemWeight> GetDropTable();
	void DropItemToPlayer();
	
};
