#include "Object.h"
#include "Component/Component.h"
#include "Manager/RenderManager.h"
int AObject::IDGenerator = 0;

AObject::AObject()
	: bIsDestroy(false)
{
	ID = IDGenerator;
	IDGenerator++;
}

AObject::~AObject()
{
}

void AObject::Tick(float DeltaTime)
{
	if (!Components.empty())
	{
		for (int i = 0; i < Components.size(); ++i)
		{
			if (Components[i])
			{
				Components[i]->Tick(DeltaTime);
			}
		}	
	}
	
	RenderManager::GetInstance()->AddRender(Position.Y, Position.X, to_string(ID));	
}

void AObject::OnSpawnFromPool()
{
	bIsDestroy = false;
}

void AObject::OnReturnToPool()
{
	bIsDestroy = true;
}
