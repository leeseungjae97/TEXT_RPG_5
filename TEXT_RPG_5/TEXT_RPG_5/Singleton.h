#pragma once
template<typename T>
class Singleton
{
private:
	static T* Instance;

public:
	static T* GetInstance();
	static void Destroy();

protected:
	Singleton() {}
	virtual ~Singleton() {}
};

template<typename T>
T* Singleton<T>::Instance = nullptr;

template<typename T>
inline T* Singleton<T>::GetInstance()
{
	if (nullptr == Instance)
	{
		Instance = new T;
	}

	return Instance;
}

template<typename T>
inline void Singleton<T>::Destroy()
{
	if (nullptr != Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}