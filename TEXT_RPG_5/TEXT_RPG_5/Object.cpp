#include "Object.h"
#include "Component/Component.h"
#include "Manager/RenderManager.h"
int AObject::IDGenerator = 0;

AObject::AObject()
	: bIsDestroy(false)
	, LastDamage(0)
	, HitFlashTime(0.0f)
	, DamageTextTime(0.0f)
	, LogTextTime(0.0f)
	, SlowTime(0.0f)
	, SlowRatio(0.35f)
{
	ID = IDGenerator;
	IDGenerator++;
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

void AObject::NotifyDamage(int Damage, float FlashDuration, float DamageTextDuration)
{
	LastDamage = Damage;
	HitFlashTime = FlashDuration;
	DamageTextTime = DamageTextDuration;
	SlowTime = 0.2f;
	NotifyLog(L"-" + to_wstring(Damage), DamageTextDuration);
}

void AObject::NotifyLog(const wstring& Text, float Duration)
{
	LogText = Text;
	LogTextTime = Duration;
}

void AObject::OnSpawnFromPool()
{
	bIsDestroy = false;
	LastDamage = 0;
	HitFlashTime = 0.0f;
	DamageTextTime = 0.0f;
	LogText.clear();
	LogTextTime = 0.0f;
	SlowTime = 0.0f;
}

void AObject::OnReturnToPool()
{
	bIsDestroy = true;
	LastDamage = 0;
	HitFlashTime = 0.0f;
	DamageTextTime = 0.0f;
	LogText.clear();
	LogTextTime = 0.0f;
	SlowTime = 0.0f;
}
