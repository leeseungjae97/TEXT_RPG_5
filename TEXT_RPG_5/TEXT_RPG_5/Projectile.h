#pragma once
#include "pch.h"
#include "Object.h"
#include "Struct/ProjectileInfo.h"

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

    void BeginPlay(AObject* InOwner, EDirection InDirection, ProjectileInfo InInfo);
    void Fire();

private:
    ProjectileInfo Info;
    EDirection Direction;

    AObject* Owner;
    int MoveCount;
    float AccTime;
    
};
