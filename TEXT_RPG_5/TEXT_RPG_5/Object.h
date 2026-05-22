#pragma once
#include "Vector.h"
#include "pch.h"

class UComponent;
class AObject
{
public:
	AObject();
	~AObject();

protected:
	bool bIsDestroy;

public:
	virtual void Init() = 0;
	virtual void Tick(float DeltaTime);
	virtual void Destroy() = 0;

public:
	bool IsDestroy() { return bIsDestroy; }

	Vector GetPosition() { return Position; }
	void SetPosition(Vector InPosition) { Position = InPosition; }

	Vector GetPrevPosition() { return PrevPosition; }
	void SetPrevPosition(Vector InPosition) { PrevPosition = InPosition; }

protected:
	Vector Position;
	Vector PrevPosition;

private:
	vector<UComponent*> Components;
};