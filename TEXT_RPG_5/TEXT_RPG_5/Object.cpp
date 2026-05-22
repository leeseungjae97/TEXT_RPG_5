#include "Object.h"
#include "Component.h"
AObject::AObject()
	: bIsDestroy(false)
{

}

AObject::~AObject()
{
	if (Components.empty())
		return;

	for (int i = 0; i < Components.size(); ++i)
	{
		if(Components[i])
			delete Components[i];
	}
}

void AObject::Tick(float DeltaTime)
{
	if (Components.empty())
		return;

	for (int i = 0; i < Components.size(); ++i)
	{
		Components[i]->Tick(DeltaTime);
	}
}
