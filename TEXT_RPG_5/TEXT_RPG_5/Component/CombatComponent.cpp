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

	AttackVisibleTime = 0.0f;
	AttackVisibleDuration = 0.18f;

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

void UCombatComponent::PlayTeleportEffect(Vector StartPosition, Vector EndPosition, float Duration)
{
	Duration = max(0.01f, Duration);
	TeleportEffects.push_back({ StartPosition, 0.0f, Duration });
	TeleportEffects.push_back({ EndPosition, 0.0f, Duration });
}

void UCombatComponent::HandleAttack(float DeltaTime)
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
	
	if (!InputManager::GetInstance()->IsKeyPressed(KeyCode::Z))
	{
		return;
	}

	float* ElapsedTime = nullptr;
	float Interval = 0.0f;
	void (UCombatComponent::*MakeRange)() = nullptr;
	bool bProjectileAttack = false;

	switch (CurrentWeapon)
	{
	case WeaponType::NONE:
		ElapsedTime = &RockElapsedTime;
		Interval = RockInterval;
		MakeRange = &UCombatComponent::MakeRockRange;
		break;

	case WeaponType::Sword:
		ElapsedTime = &SwordElapsedTime;
		Interval = SwordInterval;
		MakeRange = &UCombatComponent::MakeSwordRange;
		break;

	case WeaponType::Axe:
		ElapsedTime = &AxeElapsedTime;
		Interval = AxeInterval;
		MakeRange = &UCombatComponent::MakeAxeRange;
		break;

	case WeaponType::Bow:
		ElapsedTime = &BowElapsedTime;
		Interval = BowInterval;
		bProjectileAttack = true;
		break;

	case WeaponType::Magic:
		ElapsedTime = &MagicElapsedTime;
		Interval = MagicInterval;
		MakeRange = &UCombatComponent::MakeMagicRange;
		break;

	default:
		return;
	}

	if (ElapsedTime == nullptr || *ElapsedTime < Interval)
	{
		return;
	}

	if (bProjectileAttack)
	{
		PlayerPtr->SetIsAttack(true);
		AttackVisibleTime = AttackVisibleDuration;
		ProjectileAttack();
		*ElapsedTime = 0.0f;
		return;
	}

	if (MakeRange != nullptr && !PlayerPtr->GetIsAttack())
	{
		(this->*MakeRange)();
		Attack();
		*ElapsedTime = 0.0f;
	}
}

void UCombatComponent::Tick(float DeltaTime)
{
	for (FTeleportEffect& Effect : TeleportEffects)
	{
		Effect.Elapsed += DeltaTime;
	}

	TeleportEffects.erase(
		remove_if(TeleportEffects.begin(), TeleportEffects.end(), [](const FTeleportEffect& Effect)
			{
				return Effect.Elapsed >= Effect.Duration;
			}),
		TeleportEffects.end());

	HandleAttack(DeltaTime);
}
