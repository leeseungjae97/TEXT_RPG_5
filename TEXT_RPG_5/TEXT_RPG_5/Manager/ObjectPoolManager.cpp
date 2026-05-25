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

void ObjectPoolManager::Return(int ObjectID)
{
	Return(GetObjectByID(ObjectID));
}

AObject* ObjectPoolManager::GetObjectByID(int ObjectID)
{
	auto iter = ObjectByID.find(ObjectID);
	if (iter == ObjectByID.end())
	{
		return nullptr;
	}

	return iter->second;
}

void ObjectPoolManager::ClearAll()
{
	ObjectByID.clear();
	PoolByObject.clear();
	Pools.clear();
}
