#pragma once
#include "../Singleton.h"
#include "../pch.h"
#include "../Object.h"

class ObjectPoolManager : public Singleton<ObjectPoolManager>
{
	
public:
	ObjectPoolManager();
	~ObjectPoolManager();
	
private:
	struct ITypePool
	{
		virtual ~ITypePool() {}
		virtual bool Contains(AObject* object) const = 0;
		virtual void ReturnObject(AObject* object) = 0;
		virtual void CollectObjects(vector<AObject*>& objects) const = 0;
	};

private:
	unordered_map<type_index, unique_ptr<ITypePool>> Pools;
	
	template <typename T>
	struct TypePool : ITypePool
	{
		// 객체를 담을 배열
		vector<unique_ptr<T>> Objects;
		// 사용 가능한 객체를 담을 배열
		vector<T*> Available;
		// 객체를 담고 있는지 확인할 Set
		unordered_set<T*> AvailableSet;

		bool Contains(AObject* object) const override
		{
			for (const unique_ptr<T>& pooledObject : Objects)
			{
				if (pooledObject.get() == object)
				{
					return true;
				}
			}

			return false;
		}

		void ReturnObject(AObject* object) override
		{
			T* typedObject = static_cast<T*>(object);
			if (typedObject == nullptr || AvailableSet.count(typedObject) > 0)
			{
				return;
			}

			typedObject->OnReturnToPool();
			Available.push_back(typedObject);
			AvailableSet.insert(typedObject);
		}

		void CollectObjects(vector<AObject*>& objects) const override
		{
			for (const unique_ptr<T>& pooledObject : Objects)
			{
				objects.push_back(pooledObject.get());
			}
		}
	};

	template <typename T>
	TypePool<T>* GetPool()
	{
		const type_index key(typeid(T));
		auto iter = Pools.find(key);
		if (iter == Pools.end())
		{
			return nullptr;
		}

		return static_cast<TypePool<T>*>(iter->second.get());
	}

	template <typename T>
	const TypePool<T>* GetPool() const
	{
		const type_index key(typeid(T));
		auto iter = Pools.find(key);
		if (iter == Pools.end())
		{
			return nullptr;
		}

		return static_cast<const TypePool<T>*>(iter->second.get());
	}

	template <typename T>
	TypePool<T>* GetOrCreatePool()
	{
		TypePool<T>* pool = GetPool<T>();
		if (pool != nullptr)
		{
			return pool;
		}

		const type_index key(typeid(T));
		unique_ptr<TypePool<T>> newPool(new TypePool<T>());
		pool = newPool.get();
		Pools[key] = move(newPool);

		return pool;
	}

public:
	template <typename T, typename... Args>
	T* Get(Args&&... args)
	{
		static_assert(is_base_of<AObject, T>::value, "T must derive from AObject");

		TypePool<T>* pool = GetOrCreatePool<T>();

		if (!pool->Available.empty())
		{
			T* object = pool->Available.back();
			pool->Available.pop_back();
			pool->AvailableSet.erase(object);
			object->OnSpawnFromPool();
			return object;
		}

		unique_ptr<T> newObject(new T(std::forward<Args>(args)...));
		T* rawObject = newObject.get();
		pool->Objects.push_back(move(newObject));
		PoolByObject[rawObject] = pool;
		rawObject->OnSpawnFromPool();

		return rawObject;
	}

	template <typename T>
	void Return(T* object)
	{
		static_assert(is_base_of<AObject, T>::value, "T must derive from AObject");

		Return(static_cast<AObject*>(object));
	}

	template <typename T, typename... Args>
	void Preload(size_t Count, Args... args)
	{
		static_assert(is_base_of<AObject, T>::value, "T must derive from AObject");

		TypePool<T>* pool = GetOrCreatePool<T>();

		for (size_t i = 0; i < Count; ++i)
		{
			unique_ptr<T> newObject(new T(args...));
			T* rawObject = newObject.get();
			rawObject->OnReturnToPool();
			pool->Objects.push_back(move(newObject));
			PoolByObject[rawObject] = pool;
			pool->Available.push_back(rawObject);
			pool->AvailableSet.insert(rawObject);
		}
	}

	template <typename T>
	void Clear()
	{
		const type_index key(typeid(T));
		auto iter = Pools.find(key);
		if (iter != Pools.end())
		{
			vector<AObject*> objects;
			iter->second->CollectObjects(objects);
			for (AObject* object : objects)
			{
				PoolByObject.erase(object);
			}

			Pools.erase(iter);
		}
	}

	void Return(AObject* object);
	void ClearAll();
	size_t GetTypeCount() const { return Pools.size(); }

	template <typename T>
	size_t GetTotalCount() const
	{
		const TypePool<T>* pool = GetPool<T>();
		return pool == nullptr ? 0 : pool->Objects.size();
	}

	template <typename T>
	size_t GetAvailableCount() const
	{
		const TypePool<T>* pool = GetPool<T>();
		return pool == nullptr ? 0 : pool->Available.size();
	}

	bool Contains(AObject* object) const
	{
		return PoolByObject.find(object) != PoolByObject.end();
	}

private:
	unordered_map<AObject*, ITypePool*> PoolByObject;
};
