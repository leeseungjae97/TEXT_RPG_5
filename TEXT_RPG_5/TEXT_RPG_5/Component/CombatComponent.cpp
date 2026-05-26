#include "CombatComponent.h"
#include "MoveComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/RenderManager.h"
#include "../Player.h"
#include "../Monster.h"
#include "../Manager/SceneManager.h"
#include "../Manager/MapManager.h"
#include "../Manager/ObjectPoolManager.h"
#include "../Define.h"
#include "../Projectile.h"
#include "../Struct/ProjectileInfo.h"

UCombatComponent::UCombatComponent(AObject* InOwner)
	: UComponent(InOwner), Weapon(WeaponType::Melee)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	MoveComponentPtr = PlayerPtr != nullptr ? PlayerPtr->GetComponent<UMoveComponent>() : nullptr;
	AttackElapsedTime = 999.0f;
	AttackInterval = 2.0f;
	AttackVisibleTime = 0.0f;
	AttackVisibleDuration = 0.18f;
	bAttackRequested = false;
}

UCombatComponent::~UCombatComponent()
{
	
}

vector<Vector> UCombatComponent::GetAttackValue()
{
	return AttackValue;
}

bool UCombatComponent::IsAttackCoolingDown() const
{
	return AttackElapsedTime < AttackInterval;
}

float UCombatComponent::GetAttackCooldownAlpha() const
{
	if (AttackInterval <= 0.0f)
	{
		return 1.0f;
	}

	return min(max(AttackElapsedTime / AttackInterval, 0.0f), 1.0f);
}

void UCombatComponent::SwordAttack()
{
	if (AttackValue.empty())
		return;
	
	if (!PlayerPtr)
	{
		PlayerPtr = dynamic_cast<Player*>(GetOwner());
		return;
	}
	
	const vector<vector<Coordinate>>& Map = MapManager::GetInstance()->GetMap();
	if (Map.empty() || Map[0].empty())
	{
		return;
	}

	for (const auto AttackPos : AttackValue)
	{
		if (AttackPos.Y < 0 || AttackPos.Y >= MAP_MAX_Y || AttackPos.X < 0 || AttackPos.X >= MAP_MAX_X)
		{
			continue;
		}

		if (Map[AttackPos.Y][AttackPos.X].Type == ObjectType::Monster)
		{
			const int ID = Map[AttackPos.Y][AttackPos.X].ID;
			if (Monster* const Mon = dynamic_cast<Monster*>(ObjectPoolManager::GetInstance()->GetObjectByID(ID)))
			{
				Mon->TakeDamage(PlayerPtr->GetPower());
			}
		}
	}
}

void UCombatComponent::ProjectileAttack()
{
	if (!PlayerPtr)
	{
		PlayerPtr = dynamic_cast<Player*>(GetOwner());
		return;
	}

	if (!MoveComponentPtr)
	{
		MoveComponentPtr = PlayerPtr->GetComponent<UMoveComponent>();
		return;
	}

	EDirection ShootDirection = MoveComponentPtr->GetFacingDirection();

	if (ShootDirection == EDirection::NONE)
	{
		return;
	}

	ProjectileInfo Info;
	Info.Damage = PlayerPtr->GetPower();
	Info.Range = 7;
	Info.Speed = 0.1f;

	Projectile* NewProjectile = SceneManager::GetInstance()->SpawnObject<Projectile>();

	if (NewProjectile == nullptr)
	{
		return;
	}

	NewProjectile->BeginPlay(PlayerPtr, ShootDirection, Info);
	NewProjectile->Fire();
}

void UCombatComponent::HandleAttack()
{
	if (!bAttackRequested)
	{
		return;
	}

	switch (Weapon)
	{
		case WeaponType::Melee:
		{
			SwordAttack();
		}
		break;
		// 프로젝타일 추가
		case WeaponType::Projectile:
		{
			ProjectileAttack();
		}
		break;
		default : 
		{
			
		}
	}

	bAttackRequested = false;
}

void UCombatComponent::HandleAttackInput(float DeltaTime)
{
	AttackElapsedTime += DeltaTime;
	if (AttackVisibleTime > 0.0f)
	{
		AttackVisibleTime -= DeltaTime;
		if (AttackVisibleTime <= 0.0f)
		{
			AttackVisibleTime = 0.0f;
			AttackValue.clear();
			if (PlayerPtr != nullptr)
			{
				PlayerPtr->SetIsAttack(false);
			}
		}
	}

	if (!PlayerPtr)
	{
		PlayerPtr = dynamic_cast<Player*>(GetOwner());
		return;
	}
	
	if (!MoveComponentPtr)
	{
		MoveComponentPtr = PlayerPtr->GetComponent<UMoveComponent>();
		return;
	}
	
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::Z) && (AttackElapsedTime >= AttackInterval))
	{
		if (!PlayerPtr->GetIsAttack())
		{
			AttackValue.clear();
			PlayerPtr->SetIsAttack(true);
			Vector Pos = PlayerPtr->GetPosition();

			if (MoveComponentPtr->GetFacingDirection() == EDirection::UP)
			{
				AttackValue.push_back({ Pos.X, Pos.Y - 1 });
				AttackValue.push_back({ Pos.X - 1, Pos.Y - 1 });
				AttackValue.push_back({ Pos.X + 1, Pos.Y - 1 });
			}
			if (MoveComponentPtr->GetFacingDirection() == EDirection::DOWN)
			{
				AttackValue.push_back({ Pos.X, Pos.Y + 1 });
				AttackValue.push_back({ Pos.X - 1, Pos.Y + 1 });
				AttackValue.push_back({ Pos.X + 1, Pos.Y + 1 });
			}
			if (MoveComponentPtr->GetFacingDirection() == EDirection::RIGHT)
			{
				AttackValue.push_back({ Pos.X + 1, Pos.Y - 1 });
				AttackValue.push_back({ Pos.X + 1, Pos.Y });
				AttackValue.push_back({ Pos.X + 1, Pos.Y + 1 });
			}
			if (MoveComponentPtr->GetFacingDirection() == EDirection::LEFT)
			{
				AttackValue.push_back({ Pos.X - 1, Pos.Y - 1 });
				AttackValue.push_back({ Pos.X - 1, Pos.Y });
				AttackValue.push_back({ Pos.X - 1, Pos.Y + 1 });
			}
			if (MoveComponentPtr->GetFacingDirection() == EDirection::NONE)
			{
				AttackValue.push_back({ Pos.X, Pos.Y - 1 });
				AttackValue.push_back({ Pos.X - 1, Pos.Y });
				AttackValue.push_back({ Pos.X + 1, Pos.Y });
				AttackValue.push_back({ Pos.X, Pos.Y + 1 });
			}
			bAttackRequested = true;
			AttackVisibleTime = AttackVisibleDuration;
		}
		AttackElapsedTime = 0.0f;
	}
	// 프로젝타일 input 관련
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::X) && AttackElapsedTime >= AttackInterval)
	{
		ProjectileAttack();
		AttackElapsedTime = 0.0f;
	}
}

void UCombatComponent::Tick(float DeltaTime)
{
	HandleAttackInput(DeltaTime);
	HandleAttack();
}
