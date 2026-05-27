#include "CombatComponent.h"
#include "MoveComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/DisplayManager.h"
#include "../Player.h"
#include "../Monster.h"
#include "../Manager/SceneManager.h"
#include "../Manager/MapManager.h"
#include "../Manager/ObjectPoolManager.h"
#include "../Define.h"
#include "../Projectile.h"
#include "../Struct/ProjectileInfo.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"

UCombatComponent::UCombatComponent(AObject* InOwner)
	: UComponent(InOwner), Weapon(WeaponType::Sword)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	MoveComponentPtr = PlayerPtr != nullptr ? PlayerPtr->GetComponent<UMoveComponent>() : nullptr;
	AttackElapsedTime = 999.0f;
	AttackInterval = 1.0f;
	AttackVisibleTime = 0.0f;
	AttackVisibleDuration = 0.18f;
	bAttackRequested = false;
	ProjectileTotalTime = 0.0f;
	ProjectileDelayTime = 0.0f;
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

void UCombatComponent::Attack()
{
	if (AttackValue.empty())
		return;

	if (!PlayerPtr)
	{
		PlayerPtr = dynamic_cast<Player*>(GetOwner());
		return;
	}

	for (const auto AttackPos : AttackValue)
	{
		if (AttackPos.Y < 0 || AttackPos.Y >= MAP_MAX_Y || AttackPos.X < 0 || AttackPos.X >= MAP_MAX_X)
		{
			continue;
		}
		
		if (MapManager::GetInstance()->IsTypeExist(AttackPos.Y, AttackPos.X, MapObjectType::Monster))
		{
			const int ID = MapManager::GetInstance()->GetID(AttackPos.Y, AttackPos.X);
			if (Monster* const Mon = dynamic_cast<Monster*>(ObjectPoolManager::GetInstance()->GetObjectByID(ID)))
			{
				Mon->TakeDamage(PlayerPtr->GetPower());
			}
		}
	}
}

void UCombatComponent::TriggerCustomAttack(const vector<Vector>& CustomRange, int Damage)
{
	AttackValue = CustomRange;
	PlayerPtr->SetIsAttack(true);
	AttackVisibleTime = AttackVisibleDuration;

	for (const Vector& AttackPos : CustomRange)
	{
		if (AttackPos.Y < 0 || AttackPos.Y >= MAP_MAX_Y || AttackPos.X < 0 || AttackPos.X >= MAP_MAX_X)
			continue;

		if (MapManager::GetInstance()->IsTypeExist(AttackPos.Y, AttackPos.X, MapObjectType::Monster))
		{
			const int ID = MapManager::GetInstance()->GetID(AttackPos.Y, AttackPos.X);
			if (Monster* const Mon = dynamic_cast<Monster*>(ObjectPoolManager::GetInstance()->GetObjectByID(ID)))
			{
				Mon->TakeDamage(Damage);
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

/*void UCombatComponent::HandleAttack()
{
	if (!bAttackRequested)
	{
		return;
	}

	switch (Weapon)
	{
		case WeaponType::Sword:
		{
			SwordAttack();
		}
		break;
		case WeaponType::Bow:
		{
			ProjectileAttack();
		}
		break;
		default : 
		{
			
		}
	}

	bAttackRequested = false;
}*/
void UCombatComponent::HandleAttack()
{
	if (!bAttackRequested)
	{
		return;
	}

	if (!PlayerPtr)
	{
		PlayerPtr = dynamic_cast<Player*>(GetOwner());
		return;
	}

	UEquipmentComponent* EquipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>();

	if (EquipmentComponent == nullptr)
	{
		bAttackRequested = false;
		return;
	}

	WeaponType CurrentWeapon = EquipmentComponent->GetCurrentWeaponType();

	switch (CurrentWeapon)
	{
	case WeaponType::NONE:
		{
			Attack();
		}
		break;
		
	case WeaponType::Sword:
		{
			Attack();
		}
		break;

	case WeaponType::Axe:
		{
			Attack();
		}
		break;

	case WeaponType::Bow:
		{
			LaunchProjectile();
		}
		break;

	case WeaponType::Magic:
		{
			Attack();
		}
		break;

	default:
		break;
	}

	bAttackRequested = false;
}

void UCombatComponent::LaunchProjectile()
{
	ProjectileInfo Info;
	Info.Range = 10;
	Info.Damage = 40;
	Info.Speed = 0.1f;

	EDirection Direction = MoveComponentPtr->GetFacingDirection();

		Projectile* ProjectileAttack = ObjectPoolManager::GetInstance()->Get<Projectile>();

		if (ProjectileAttack != nullptr)
		{
			SceneManager::GetInstance()->AddObject(ProjectileAttack);
			ProjectileAttack->BeginPlay(PlayerPtr, Direction, Info);
			ProjectileAttack->Fire();
		}
			
	ProjectileTotalTime = 0.0f;
	ProjectileDelayTime = 0.5f;
}

void UCombatComponent::MakeSwordRange()
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

// Axe
void UCombatComponent::MakeAxeRange()
{
	AttackValue.clear();
	PlayerPtr->SetIsAttack(true);
	Vector Pos = PlayerPtr->GetPosition();

	if (MoveComponentPtr->GetFacingDirection() == EDirection::UP)
	{
		AttackValue.push_back({ Pos.X, Pos.Y - 1 });
		AttackValue.push_back({ Pos.X, Pos.Y - 2 });
		AttackValue.push_back({ Pos.X, Pos.Y - 3 });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::DOWN)
	{
		AttackValue.push_back({ Pos.X, Pos.Y + 1 });
		AttackValue.push_back({ Pos.X, Pos.Y + 2 });
		AttackValue.push_back({ Pos.X, Pos.Y + 3 });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::RIGHT)
	{
		AttackValue.push_back({ Pos.X + 1, Pos.Y });
		AttackValue.push_back({ Pos.X + 2, Pos.Y });
		AttackValue.push_back({ Pos.X + 3, Pos.Y });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::LEFT)
	{
		AttackValue.push_back({ Pos.X - 1, Pos.Y });
		AttackValue.push_back({ Pos.X - 2, Pos.Y });
		AttackValue.push_back({ Pos.X - 3, Pos.Y });
	}

	bAttackRequested = true;
	AttackVisibleTime = AttackVisibleDuration;
}

// 주먹
void UCombatComponent::MakeRockRange()
{
	AttackValue.clear();
	PlayerPtr->SetIsAttack(true);
	Vector Pos = PlayerPtr->GetPosition();

	if (MoveComponentPtr->GetFacingDirection() == EDirection::UP)
	{
		AttackValue.push_back({ Pos.X, Pos.Y - 1 });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::DOWN)
	{
		AttackValue.push_back({ Pos.X, Pos.Y + 1 });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::RIGHT)
	{
		AttackValue.push_back({ Pos.X + 1, Pos.Y });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::LEFT)
	{
		AttackValue.push_back({ Pos.X - 1, Pos.Y });
	}

	bAttackRequested = true;
	AttackVisibleTime = AttackVisibleDuration;
}

// 지팡이
void UCombatComponent::MakeMagicRange()
{
	AttackValue.clear();
	PlayerPtr->SetIsAttack(true);
	Vector Pos = PlayerPtr->GetPosition();

	if (MoveComponentPtr->GetFacingDirection() == EDirection::UP)
	{
		AttackValue.push_back({ Pos.X, Pos.Y - 1 });
		AttackValue.push_back({ Pos.X - 1, Pos.Y - 2 });
		AttackValue.push_back({ Pos.X, Pos.Y - 2 });
		AttackValue.push_back({ Pos.X + 1, Pos.Y - 2 });
		AttackValue.push_back({ Pos.X-2, Pos.Y - 3 });
		AttackValue.push_back({ Pos.X-1, Pos.Y - 3 });
		AttackValue.push_back({ Pos.X, Pos.Y - 3 });
		AttackValue.push_back({ Pos.X+1, Pos.Y - 3 });
		AttackValue.push_back({ Pos.X+2, Pos.Y - 3 });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::DOWN)
	{
		AttackValue.push_back({ Pos.X, Pos.Y + 1 });
		AttackValue.push_back({ Pos.X - 1, Pos.Y + 2 });
		AttackValue.push_back({ Pos.X, Pos.Y + 2 });
		AttackValue.push_back({ Pos.X + 1, Pos.Y + 2 });
		AttackValue.push_back({ Pos.X - 2, Pos.Y + 3 });
		AttackValue.push_back({ Pos.X - 1, Pos.Y + 3 });
		AttackValue.push_back({ Pos.X, Pos.Y + 3 });
		AttackValue.push_back({ Pos.X + 1, Pos.Y + 3 });
		AttackValue.push_back({ Pos.X + 2, Pos.Y + 3 });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::RIGHT)
	{
		AttackValue.push_back({ Pos.X + 1, Pos.Y  });
		AttackValue.push_back({ Pos.X + 2, Pos.Y - 1 });
		AttackValue.push_back({ Pos.X + 2, Pos.Y  });
		AttackValue.push_back({ Pos.X + 2, Pos.Y + 1 });
		AttackValue.push_back({ Pos.X + 3, Pos.Y - 2 });
		AttackValue.push_back({ Pos.X + 3, Pos.Y - 1 });
		AttackValue.push_back({ Pos.X + 3, Pos.Y });
		AttackValue.push_back({ Pos.X + 3, Pos.Y + 1 });
		AttackValue.push_back({ Pos.X + 3, Pos.Y + 2 });
	}
	if (MoveComponentPtr->GetFacingDirection() == EDirection::LEFT)
	{
		AttackValue.push_back({ Pos.X - 1, Pos.Y  });
		AttackValue.push_back({ Pos.X - 2, Pos.Y - 1 });
		AttackValue.push_back({ Pos.X - 2, Pos.Y  });
		AttackValue.push_back({ Pos.X - 2, Pos.Y + 1 });
		AttackValue.push_back({ Pos.X - 3, Pos.Y - 2 });
		AttackValue.push_back({ Pos.X - 3, Pos.Y - 1 });
		AttackValue.push_back({ Pos.X - 3, Pos.Y });
		AttackValue.push_back({ Pos.X - 3, Pos.Y + 1 });
		AttackValue.push_back({ Pos.X - 3, Pos.Y + 2 });
	}

	bAttackRequested = true;
	AttackVisibleTime = AttackVisibleDuration;
}

void UCombatComponent::AttackEffectTimeAcc(float DeltaTime)
{
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
}

void UCombatComponent::HandleAttackInput(float DeltaTime)
{
	AttackElapsedTime += DeltaTime;
	ProjectileTotalTime += DeltaTime;
	
	AttackEffectTimeAcc(DeltaTime);

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
	
	if (UInventoryComponent* InventoryComponent = PlayerPtr->GetComponent<UInventoryComponent>())
	{
		if (InventoryComponent->GetOpenedInventory())
		{
			return;
		}
	}
	
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::A) && (AttackElapsedTime >= AttackInterval))
	{
		UEquipmentComponent* EquipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>();
		
		if (EquipmentComponent == nullptr)
		{
			return;
		}
		
		WeaponType CurrentWeapon = EquipmentComponent->GetCurrentWeaponType();

		switch (CurrentWeapon)
		{
		case WeaponType::NONE:
			{
				if (!PlayerPtr->GetIsAttack())
				{
					MakeRockRange();
					AttackElapsedTime = 0.0f;
				}
			}
			break;
		case WeaponType::Sword:
			{
				if (!PlayerPtr->GetIsAttack())
				{
					MakeSwordRange();
					AttackElapsedTime = 0.0f;
				}
			}
			break;

		case WeaponType::Axe:
			{
				if (!PlayerPtr->GetIsAttack())
				{
					MakeAxeRange();
					AttackElapsedTime = 0.0f;
				}
			}
			break;

		case WeaponType::Bow:
			{
				LaunchProjectile();
				AttackElapsedTime = 0.0f;
			}
			break;
		case WeaponType::Magic:
			{
				MakeMagicRange();
				AttackElapsedTime = 0.0f;
			}
			break;

		default:
			{
				
			}
			break;
		}
	}
}	

/*if (!PlayerPtr->GetIsAttack())
		{
			MakeSwordRange();
		}
		AttackElapsedTime = 0.0f;
	}
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::X) && (ProjectileTotalTime >= ProjectileDelayTime))
	{
		LaunchProjectile();
	}*/

void UCombatComponent::Tick(float DeltaTime)
{
	HandleAttackInput(DeltaTime);
	HandleAttack();
}
