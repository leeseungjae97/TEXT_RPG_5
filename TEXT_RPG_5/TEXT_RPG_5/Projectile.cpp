#include "Projectile.h"
#include "Component/MoveComponent.h"
#include "Struct/Coordinate.h"
#include "Manager/MapManager.h"
#include "Manager/ObjectPoolManager.h"
#include "Monster.h"

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

void Projectile::Fire()
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
        if (CurrentTile.Type == ObjectType::Wall)
        {
            break;
        }

        // 플레이어는 Monster만 공격
        if (CurrentTile.Type == ObjectType::Monster)
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
}

void Projectile::BeginPlay(AObject* InOwner, EDirection InDirection, ProjectileInfo InInfo)
{
    Owner = InOwner;
    Direction = InDirection;
    Info = InInfo;
}

void Projectile::BeginPlay()
{

}

void Projectile::Destroy()
{

}

void Projectile::Tick(float DeltaTime)
{
    AObject::Tick(DeltaTime);
}
