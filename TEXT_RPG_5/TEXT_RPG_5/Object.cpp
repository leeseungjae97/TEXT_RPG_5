#include "Object.h"
#include "Component/Component.h"
#include "Manager/MapManager.h"
#include "Manager/DisplayManager.h"
int AObject::IDGenerator = 0;

AObject::AObject()
	: bIsDestroy(false)
	, bIsMoving(false)
	, bPositionCommitted(false)
	, LastDamage(0)
	, HitFlashTime(0.0f)
	, HitEffect(HitEffectType::None)
	, HitEffectTime(0.0f)
	, DamageTextTime(0.0f)
	, LogTextTime(0.0f)
	, SlowTime(0.0f)
	, SlowRatio(0.35f)
{
	ID = IDGenerator;
	IDGenerator++;
	NextPosition = Position;
}

AObject::~AObject()
{
}

void AObject::TimeAdd(float DeltaTime)
{
	if (HitFlashTime > 0.0f)
	{
		HitFlashTime -= DeltaTime;
		if (HitFlashTime < 0.0f)
		{
			HitFlashTime = 0.0f;
		}
	}

	if (HitEffectTime > 0.0f)
	{
		HitEffectTime -= DeltaTime;
		if (HitEffectTime < 0.0f)
		{
			HitEffectTime = 0.0f;
			HitEffect = HitEffectType::None;
		}
	}

	if (DamageTextTime > 0.0f)
	{
		DamageTextTime -= DeltaTime;
		if (DamageTextTime < 0.0f)
		{
			DamageTextTime = 0.0f;
		}
	}

	if (LogTextTime > 0.0f)
	{
		LogTextTime -= DeltaTime;
		if (LogTextTime < 0.0f)
		{
			LogTextTime = 0.0f;
			LogText.clear();
		}
	}

	if (SlowTime > 0.0f)
	{
		SlowTime -= DeltaTime;
		if (SlowTime < 0.0f)
		{
			SlowTime = 0.0f;
		}
	}
}

void AObject::Tick(float DeltaTime)
{
	if (bIsDestroy)
	{
		return;
	}

	TimeAdd(DeltaTime);

	const float ScaledDeltaTime = DeltaTime * GetSlowRatio();

	if (!Components.empty())
	{
		for (int i = 0; i < Components.size(); ++i)
		{
			if (Components[i])
			{
				Components[i]->Tick(ScaledDeltaTime);
			}
		}	
	}
}

void AObject::SetPosition(Vector InPosition)
{
	Vector OldPosition = Position;

	Position = InPosition;
	PrevPosition = InPosition;
	NextPosition = InPosition;
	bIsMoving = false;
	bPositionCommitted = false;

	if (!bIsDestroy && MapManager::GetInstance()->IsMapInitSize())
	{
		MapManager::GetInstance()->MoveObject(this, OldPosition, Position);
	}
}

void AObject::BeginMoveTo(Vector InNextPosition)
{
	if (bIsDestroy || bIsMoving || (InNextPosition.X == Position.X && InNextPosition.Y == Position.Y))
	{
		return;
	}

	PrevPosition = Position;
	NextPosition = InNextPosition;
	bIsMoving = true;
	bPositionCommitted = false;
}

bool AObject::CommitMoveIfNeeded(float Alpha)
{
	if (bIsDestroy || !bIsMoving || bPositionCommitted || Alpha < 0.5f)
	{
		return false;
	}

	if (MapManager::GetInstance()->IsMapInitSize())
	{
		if (!MapManager::GetInstance()->MoveObject(this, Position, NextPosition))
		{
			NextPosition = Position;
			PrevPosition = Position;
			bIsMoving = false;
			bPositionCommitted = false;
			return false;
		}
	}

	Position = NextPosition;
	bPositionCommitted = true;
	return true;
}

void AObject::FinishMoveIfNeeded(float Alpha)
{
	if (bIsDestroy || !bIsMoving || Alpha < 1.0f)
	{
		return;
	}

	if (!bPositionCommitted)
	{
		CommitMoveIfNeeded(1.0f);
	}

	PrevPosition = Position;
	NextPosition = Position;
	bIsMoving = false;
	bPositionCommitted = false;
}

void AObject::NotifyDamage(int Damage, float FlashDuration, float DamageTextDuration)
{
	LastDamage = Damage;
	HitFlashTime = FlashDuration;
	DamageTextTime = DamageTextDuration;
	SlowTime = 0.2f;
	NotifyLog(L"-" + to_wstring(Damage), DamageTextDuration);
}

void AObject::NotifyHitEffect(HitEffectType Type, float Duration)
{
	HitEffect = Type;
	HitEffectTime = Duration;
}

void AObject::NotifyLog(const wstring& Text, float Duration)
{
	LogText = Text;
	LogTextTime = Duration;
}

void AObject::OnSpawnFromPool()
{
	bIsDestroy = false;
	NextPosition = Position;
	bIsMoving = false;
	bPositionCommitted = false;
	LastDamage = 0;
	HitFlashTime = 0.0f;
	HitEffect = HitEffectType::None;
	HitEffectTime = 0.0f;
	DamageTextTime = 0.0f;
	LogText.clear();
	LogTextTime = 0.0f;
	SlowTime = 0.0f;
}

void AObject::OnReturnToPool()
{
	if (MapManager::GetInstance()->IsMapInitSize())
	{
		MapManager::GetInstance()->ClearObject(this);
	}

	bIsDestroy = true;
	NextPosition = Position;
	bIsMoving = false;
	bPositionCommitted = false;
	LastDamage = 0;
	HitFlashTime = 0.0f;
	HitEffect = HitEffectType::None;
	HitEffectTime = 0.0f;
	DamageTextTime = 0.0f;
	LogText.clear();
	LogTextTime = 0.0f;
	SlowTime = 0.0f;
}
