#pragma once
#include "Singleton.h"
#include "pch.h"

class TimeManager : public Singleton<TimeManager>
{
public:
	TimeManager();
	~TimeManager();

private:
	static double DeltaTime;
	static double StoreSecond;
	static UINT FrameCount;
	
	static LARGE_INTEGER CpuFrequency;
	static LARGE_INTEGER PrevFrequency;
	static LARGE_INTEGER CurFrequency;
public:
	double GetDeltaTime() { return DeltaTime; }
	static UINT LastFPS;

public:
	void Tick();
	void Init();
	void CalcFPS();
};