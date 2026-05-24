#pragma once
#include "Object.h"

class UCombatComponent;
class NPC : public AObject
{
public:
	NPC();
	~NPC();

	UCombatComponent* CombatComponent;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroy() override;
};
