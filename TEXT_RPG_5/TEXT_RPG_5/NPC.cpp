#include "NPC.h"
#include "Component/CombatComponent.h"

NPC::NPC()
{
	CombatComponent = CreateDefaultComponent<UCombatComponent>();
}

NPC::~NPC()
{
}

void NPC::BeginPlay()
{
	
}

void NPC::Tick(float DeltaTime)
{

}

void NPC::Destroy()
{
	bIsDestroy = true;
}
