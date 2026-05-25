#pragma once
#include "../Singleton.h"
#include "../pch.h"

class TimeManager : public Singleton<TimeManager>
{
public:
	TimeManager();
	~TimeManager();

private:
	double DeltaTime;
	double StoreSecond;
	UINT FrameCount;
	UINT LastFPS;
	
	LARGE_INTEGER CpuFrequency;
	LARGE_INTEGER PrevFrequency;
	LARGE_INTEGER CurFrequency;
public:
	double GetDeltaTime() { return DeltaTime; }

public:
	void Tick();
	void BeginPlay();
	void CalcFPS();
};
