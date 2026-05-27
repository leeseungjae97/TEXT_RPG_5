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

/*UCombatComponent::UCombatComponent(AObject* InOwner)
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
	CurrentAttackInterval = AttackInterval;
}*/

UCombatComponent::UCombatComponent(AObject* InOwner)
	: UComponent(InOwner), Weapon(WeaponType::Sword)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	MoveComponentPtr = PlayerPtr != nullptr ? PlayerPtr->GetComponent<UMoveComponent>() : nullptr;

	AttackVisibleTime = 0.0f;
	AttackVisibleDuration = 0.18f;
	bAttackRequested = false;

	RockInterval = 0.8f;
	SwordInterval = 1.3f;
	AxeInterval = 2.0f;
	BowInterval = 1.0f;
	MagicInterval = 1.0f;

	RockElapsedTime = RockInterval;
	SwordElapsedTime = SwordInterval;
	AxeElapsedTime = AxeInterval;
	BowElapsedTime = BowInterval;
	MagicElapsedTime = MagicInterval;
}

UCombatComponent::~UCombatComponent()
{
	
}

vector<Vector> UCombatComponent::GetAttackValue()
{
	return AttackValue;
}

/*bool UCombatComponent::IsAttackCoolingDown() const
{
	return AttackElapsedTime < AttackInterval;
}*/

bool UCombatComponent::IsAttackCoolingDown() const
{
	WeaponType CurrentWeapon = WeaponType::NONE;

	if (PlayerPtr != nullptr)
	{
		UEquipmentComponent* EquipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>();

		if (EquipmentComponent != nullptr)
		{
			CurrentWeapon = EquipmentComponent->GetCurrentWeaponType();
		}
	}

	switch (CurrentWeapon)
	{
	case WeaponType::NONE:
		return RockElapsedTime < RockInterval;

	case WeaponType::Sword:
		return SwordElapsedTime < SwordInterval;

	case WeaponType::Axe:
		return AxeElapsedTime < AxeInterval;

	case WeaponType::Bow:
		return BowElapsedTime < BowInterval;

	case WeaponType::Magic:
		return MagicElapsedTime < MagicInterval;

	default:
		return false;
	}
}

/*
float UCombatComponent::GetAttackCooldownAlpha() const
{
	if (AttackInterval <= 0.0f)
	{
		return 1.0f;
	}

	return min(max(AttackElapsedTime / AttackInterval, 0.0f), 1.0f);
}
*/
float UCombatComponent::GetAttackCooldownAlpha() const
{
	WeaponType CurrentWeapon = WeaponType::NONE;

	if (PlayerPtr != nullptr)
	{
		UEquipmentComponent* EquipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>();

		if (EquipmentComponent != nullptr)
		{
			CurrentWeapon = EquipmentComponent->GetCurrentWeaponType();
		}
	}

	float ElapsedTime = 0.0f;
	float Interval = 1.0f;

	switch (CurrentWeapon)
	{
	case WeaponType::NONE:
		ElapsedTime = RockElapsedTime;
		Interval = RockInterval;
		break;

	case WeaponType::Sword:
		ElapsedTime = SwordElapsedTime;
		Interval = SwordInterval;
		break;

	case WeaponType::Axe:
		ElapsedTime = AxeElapsedTime;
		Interval = AxeInterval;
		break;

	case WeaponType::Bow:
		ElapsedTime = BowElapsedTime;
		Interval = BowInterval;
		break;

	case WeaponType::Magic:
		ElapsedTime = MagicElapsedTime;
		Interval = MagicInterval;
		break;

	default:
		return 1.0f;
	}

	if (Interval <= 0.0f)
	{
		return 1.0f;
	}

	return min(max(ElapsedTime / Interval, 0.0f), 1.0f);
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

	WeaponType CurrentWeapon = WeaponType::NONE;
	if (UEquipmentComponent* EquipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>())
	{
		CurrentWeapon = EquipmentComponent->GetCurrentWeaponType();
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
				if (CurrentWeapon == WeaponType::Magic)
				{
					Mon->NotifyHitEffect(HitEffectType::Magic, 0.45f);
				}
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

float UCombatComponent::GetAttackAnimationAlpha() const
{
	if (AttackVisibleDuration <= 0.0f || AttackVisibleTime <= 0.0f)
	{
		return 1.0f;
	}

	return min(max(1.0f - AttackVisibleTime / AttackVisibleDuration, 0.0f), 1.0f);
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
			
	/*ProjectileTotalTime = 0.0f;
	ProjectileDelayTime = 0.5f;*/
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
	RockElapsedTime += DeltaTime;
	SwordElapsedTime += DeltaTime;
	AxeElapsedTime += DeltaTime;
	BowElapsedTime += DeltaTime;
	MagicElapsedTime += DeltaTime;
	
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
	
	UEquipmentComponent* EquipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>();
		
	if (EquipmentComponent == nullptr)
	{
		return;
	}
	
	WeaponType CurrentWeapon = EquipmentComponent->GetCurrentWeaponType();
	
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::Z))
	{
		switch (CurrentWeapon)
		{
		case WeaponType::NONE:
			{
				if (RockElapsedTime >= RockInterval && !PlayerPtr->GetIsAttack())
				{
					MakeRockRange();
					RockElapsedTime = 0.0f;
				}
			}
			break;

		case WeaponType::Sword:
			{
				if (SwordElapsedTime >= SwordInterval && !PlayerPtr->GetIsAttack())
				{
					MakeSwordRange();
					SwordElapsedTime = 0.0f;
				}
			}
			break;

		case WeaponType::Axe:
			{
				if (AxeElapsedTime >= AxeInterval && !PlayerPtr->GetIsAttack())
				{
					MakeAxeRange();
					AxeElapsedTime = 0.0f;
				}
			}
			break;

		case WeaponType::Bow:
			{
				if (BowElapsedTime >= BowInterval)
				{
					PlayerPtr->SetIsAttack(true);
					AttackVisibleTime = AttackVisibleDuration;
					LaunchProjectile();
					BowElapsedTime = 0.0f;
				}
			}
			break;

		case WeaponType::Magic:
			{
				if (MagicElapsedTime >= MagicInterval && !PlayerPtr->GetIsAttack())
				{
					MakeMagicRange();
					MagicElapsedTime = 0.0f;
				}
			}
			break;

		default:
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
