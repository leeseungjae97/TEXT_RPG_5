#include "Projectile.h"
#include "Component/MoveComponent.h"
#include "Struct/Coordinate.h"
#include "Manager/MapManager.h"
#include "Manager/ObjectPoolManager.h"
#include "Monster.h"
#include "Define.h"


Projectile::Projectile()
    : Owner(nullptr)
    , Direction(EDirection::NONE)
{
    Info.Range = 0;
    Info.Damage = 0;
    Info.Speed = 0;
    this->MoveCount = 0;
    this->AccTime = 0.0f;
}

Projectile::Projectile(AObject* Owner, EDirection Direction, ProjectileInfo Info) : Owner(Owner), Direction(Direction)
{
    this->Info.Range = Info.Range;
    this->Info.Damage = Info.Damage;
    this->Info.Speed = Info.Speed;
    this->MoveCount = 0;
    this->AccTime = 0.0f;
}
Projectile::~Projectile()
{
    
}

// 기존의 라인 트레이서 느낌
/*void Projectile::Fire()
{
    if (Owner == nullptr)
    {
        return;
    }

    vector<vector<Coordinate>>& Map = MapManager::GetInstance()->GetMap();

    Vector CheckPosition = Owner->GetPosition();

    for (int i = 0; i < Info.Range; ++i)
    {
        switch (Direction)
        {
        case EDirection::UP:
            CheckPosition.Y -= 1;
            break;

        case EDirection::DOWN:
            CheckPosition.Y += 1;
            break;

        case EDirection::LEFT:
            CheckPosition.X -= 1;
            break;

        case EDirection::RIGHT:
            CheckPosition.X += 1;
            break;

        case EDirection::NONE:
            Destroy();
            return;
        }

        Coordinate& CurrentTile = Map[CheckPosition.Y][CheckPosition.X];

        //RenderManager::GetInstance()->AddRender(0, 0, "Tile Type : " + to_string(static_cast<int>(CurrentTile.Type)));
        
        // 벽 -> 공격 x
        if (CurrentTile.Type == MapObjectType::Wall)
        {
            break;
        }

        // 플레이어는 Monster만 공격
        if (CurrentTile.Type == MapObjectType::Monster)
        {
            int ObjectID = CurrentTile.ID;

            AObject* HitObject = ObjectPoolManager::GetInstance()->GetObjectByID(ObjectID);

            // 플레이어가 쏜거에 플레이어 맞기 불가
            if (HitObject == Owner)
            {
                continue;
            }
            
            // 몬스터에게 데미지 주기
            if (Monster* Mons = dynamic_cast<Monster*>(HitObject))
            {
                //RenderManager::GetInstance()->AddRender(0, 0, "Projectile Fire Start");
                Mons->TakeDamage(Info.Damage);
            }
            break;
        }
    }
}*/

void Projectile::Fire()
{
    if (Owner == nullptr)
    {
        return;
    }

    Position = Owner->GetPosition();
    PrevPosition = Position;

    MoveCount = 0;
    
    AccTime = Info.Speed;

    bActive = true;
    bIsDestroy = false;
}

void Projectile::BeginPlay(AObject* InOwner, EDirection InDirection, ProjectileInfo InInfo)
{
    Owner = InOwner;
    Direction = InDirection;
    Info = InInfo;
    
    Position = Owner->GetPosition();
    PrevPosition = Position;

    MoveCount = 0;
    AccTime = 0.0f;
    bActive = true;
    
    bHasPreviousTile = false;
}

void Projectile::BeginPlay()
{

}

void Projectile::Tick(float DeltaTime)
{
    AObject::Tick(DeltaTime);

    if (!CanTickProjectile())
    {
        return;
    }

    AccTime += DeltaTime;

    if (!CanMove())
    {
        return;
    }

    AccTime = 0.0f;

    MoveProjectile();
}

void Projectile::Destroy()
{
    bActive = false;
    bIsDestroy = true;
}

bool Projectile::CanTickProjectile()
{
    if (!bActive)
    {
        return false;
    }

    if (Owner == nullptr)
    {
        Destroy();
        return false;
    }
    
    if (!MapManager::GetInstance()->IsMapInitSize())
    {
        Destroy();
        return false;
    }

    return true;
}

bool Projectile::CanMove() const
{
    return AccTime >= Info.Speed;
}

void Projectile::MoveProjectile()
{
    PrevPosition = Position;

    MoveByDirection();

    MoveCount++;

    if (IsOutOfMap())
    {
        Destroy();
        return;
    }

    if (HandleCurrentTile())
    {
        return;
    }

    if (MoveCount >= Info.Range)
    {
        Destroy();
        return;
    }
}

void Projectile::MoveByDirection()
{
    switch (Direction)
    {
    case EDirection::UP:
        Position.Y -= 1;
        break;

    case EDirection::DOWN:
        Position.Y += 1;
        break;

    case EDirection::LEFT:
        Position.X -= 1;
        break;

    case EDirection::RIGHT:
        Position.X += 1;
        break;

    case EDirection::NONE:
    default:
        Destroy();
        break;
    }
}

bool Projectile::IsOutOfMap() const
{
    return Position.Y < 0 || Position.Y >= MAP_MAX_Y ||
           Position.X < 0 || Position.X >= MAP_MAX_X;
}

bool Projectile::HandleCurrentTile()
{
    if (MapManager::GetInstance()->IsTypeExist(Position, MapObjectType::Wall))
    {
        Destroy();
        return true;
    }

    if (AObject* Obj = MapManager::GetInstance()->GetMapObject(Position, MapObjectType::Monster))
    {
        if (Monster* Mons = dynamic_cast<Monster*>(Obj))
        {
            Mons->TakeDamage(Info.Damage);
        }
        Destroy();
        return true;
    }
    
    return false;
}

void Projectile::OnSpawnFromPool()
{
    AObject::OnSpawnFromPool();
    Owner = nullptr;
}
