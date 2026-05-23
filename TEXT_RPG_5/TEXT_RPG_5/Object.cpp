#include "Object.h"
#include "Component.h"
AObject::AObject()
	: bIsDestroy(false)
{
	Components.resize((int)ComponentType::MAX);
}

AObject::~AObject()
{
}

void AObject::Tick(float DeltaTime)
{
	if (Components.empty())
		return;

	for (int i = 0; i < Components.size(); ++i)
	{
		if (Components[i])
		{
			Components[i]->Tick(DeltaTime);
		}
	}
}
