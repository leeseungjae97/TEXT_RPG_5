#pragma once

#include "pch.h"
#include "Struct/Vector.h"
#include "Enum/ComponentTypeEnum.h"

class UComponent;

class AObject
{
public:
	AObject();
	virtual ~AObject();

protected:
	bool bIsDestroy;

public:
	virtual void BeginPlay() = 0;
	virtual void Tick(float DeltaTime);
	virtual void Destroy() = 0;
	virtual void OnSpawnFromPool();
	virtual void OnReturnToPool();

public:
	bool IsDestroy() const { return bIsDestroy; }
	bool IsHitFlashActive() const { return HitFlashTime > 0.0f; }
	bool ShouldShowDamageText() const { return DamageTextTime > 0.0f; }
	int GetLastDamage() const { return LastDamage; }
	void NotifyDamage(int Damage, float FlashDuration = 0.25f, float DamageTextDuration = 3.0f);
	bool IsSlowed() const { return SlowTime > 0.0f; }
	float GetSlowRatio() const { return IsSlowed() ? SlowRatio : 1.0f; }

	Vector GetPosition() const { return Position; }
	void SetPosition(Vector InPosition) { Position = InPosition; }

	Vector GetPrevPosition() const { return PrevPosition; }
	void SetPrevPosition(Vector InPosition) { PrevPosition = InPosition; }

	template <typename T>
	T* CreateDefaultComponent()
	{
		static_assert(is_base_of<UComponent, T>::value, "T must derive from UComponent");

		if (Components.empty())
			Components.resize((int)ComponentType::MAX);

		const int Index = static_cast<int>(T::Type);
		if (Index < 0 || Index >= static_cast<int>(Components.size()))
		{
			return nullptr;
		}

		if (Components[Index])
		{
			return static_cast<T*>(Components[Index].get());
		}

		unique_ptr<T> NewComponent(new T(this));
		T* RawPtr = NewComponent.get();

		Components[Index] = move(NewComponent);
		return RawPtr;
	}

	template <typename T>
	T* GetComponent()
	{
		static_assert(is_base_of<UComponent, T>::value, "T must derive from UComponent");

		const int Index = static_cast<int>(T::Type);
		if (Index < 0 || Index >= static_cast<int>(Components.size()))
		{
			return nullptr;
		}

		if (!Components[Index])
		{
			return nullptr;
		}

		return static_cast<T*>(Components[Index].get());
	}

protected:
	Vector Position;
	Vector PrevPosition;
	int LastDamage;
	float HitFlashTime;
	float DamageTextTime;
	float SlowTime;
	float SlowRatio;

	static int IDGenerator;
	int ID;
	
public:
	const int GetID() const {return ID;}
	
private:
	vector<unique_ptr<UComponent>> Components;
};
