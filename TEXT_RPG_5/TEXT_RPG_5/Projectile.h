#pragma once
#include "pch.h"
#include "Object.h"
#include "Struct/ProjectileInfo.h"
#include "Struct/Coordinate.h"

enum class EDirection : int;

class Projectile : public AObject
{
public:
    Projectile();
    Projectile(AObject* Owner, EDirection Direction, ProjectileInfo Info);
    virtual ~Projectile();

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroy() override;
    virtual void OnSpawnFromPool() override;

    void BeginPlay(AObject* InOwner, EDirection InDirection, ProjectileInfo InInfo);
    void Fire();
    EDirection GetDirection() const { return Direction; }

private:
    // Tick 흐름 제어
    bool CanTickProjectile();
    bool CanMove() const;

    // 이동 처리
    void MoveProjectile();
    void MoveByDirection();

    // 맵 범위 확인
    bool IsOutOfMap() const;

    // 충돌 처리
    bool HandleCurrentTile();

private:
    ProjectileInfo Info;
    EDirection Direction;

    AObject* Owner;
    
    int MoveCount = 0;
    float AccTime = 0.0f;
    bool bActive = false;
    Coordinate PreviousTile;
    bool bHasPreviousTile = false;
};
