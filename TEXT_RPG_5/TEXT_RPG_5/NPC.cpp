#include "NPC.h"
#include "CombatComponent.h"

NPC::NPC()
{
	CombatComponent = new UCombatComponent(this);
}

NPC::~NPC()
{
}

void NPC::Init()
{
	
}

void NPC::Tick(float DeltaTime)
{

}

void NPC::Destroy()
{
	bIsDestroy = true;
}
