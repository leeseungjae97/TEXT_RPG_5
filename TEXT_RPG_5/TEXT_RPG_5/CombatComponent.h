#pragma once

#include "Component.h"

class UCombatComponent : public UComponent
{
public:
	UCombatComponent(AObject* InOwner);
	~UCombatComponent();

public:
	virtual void Tick(float DeltaTime) override;

};
