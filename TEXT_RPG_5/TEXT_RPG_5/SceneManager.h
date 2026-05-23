#pragma once
#include "Singleton.h"
#include "pch.h"

class AObject;
class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

public:
	void Tick(float DeltaTime);
	void BeginPlay();
	void Destroy();

public:
	void AddObject(AObject* object);
	void RemoveObject(AObject* object);
	vector<AObject*>& GetObjects() { return Objects; }

private:
	vector<AObject*> Objects;
};
