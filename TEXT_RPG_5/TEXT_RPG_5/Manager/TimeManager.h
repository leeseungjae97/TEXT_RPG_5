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
	double StoredTime;
	UINT FrameCount;
	UINT LastFPS;
	
	LARGE_INTEGER CpuFrequency;
	LARGE_INTEGER PrevFrequency;
	LARGE_INTEGER CurFrequency;
public:
	double GetDeltaTime() { return DeltaTime; }
	double GetStoredTime() const { return StoredTime; }
	void ResetStoredTime() { StoredTime = 0.0; }
	void AddStoredTime(double InDeltaTime) { StoredTime += max(0.0, InDeltaTime); }
	wstring GetFormattedStoredTime() const { return FormatSeconds(StoredTime); }
	static wstring FormatSeconds(double Seconds);

public:
	void Tick();
	void BeginPlay();
	void CalcFPS();
};
