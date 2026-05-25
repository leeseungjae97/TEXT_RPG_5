#include "ObjectPoolManager.h"

ObjectPoolManager::ObjectPoolManager()
{
}

ObjectPoolManager::~ObjectPoolManager()
{
	ClearAll();
}

void ObjectPoolManager::Return(AObject* object)
{
	if (object == nullptr)
	{
		return;
	}

	auto iter = PoolByObject.find(object);
	if (iter == PoolByObject.end() || iter->second == nullptr)
	{
		return;
	}

	iter->second->ReturnObject(object);
}

void ObjectPoolManager::ClearAll()
{
	PoolByObject.clear();
	Pools.clear();
}
