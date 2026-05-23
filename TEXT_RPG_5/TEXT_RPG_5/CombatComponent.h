#pragma once

#include "Component.h"
#include "ComponentTypeEnum.h"

class UCombatComponent : public UComponent
{
	friend class AObject;

public:
	static constexpr ComponentType Type = ComponentType::CombatComponent;

	~UCombatComponent();

public:
	virtual void Tick(float DeltaTime) override;

private:
	UCombatComponent(AObject* InOwner);
};
