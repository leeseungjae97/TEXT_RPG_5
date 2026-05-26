#include "TimeManager.h"

#include "RenderManager.h"

TimeManager::TimeManager()
	: DeltaTime(0.f), StoreSecond(0.0f), FrameCount(0), LastFPS(0),
	CpuFrequency(), PrevFrequency(), CurFrequency()
{
}

TimeManager::~TimeManager()
{
}

void TimeManager::Tick()
{
	if (CpuFrequency.QuadPart == 0)
	{
		BeginPlay();
	}

	QueryPerformanceCounter(&CurFrequency);

	double differnceFrequency = CurFrequency.QuadPart - PrevFrequency.QuadPart;
	DeltaTime = differnceFrequency / static_cast<double>(CpuFrequency.QuadPart);
	PrevFrequency.QuadPart = CurFrequency.QuadPart;
	
	CalcFPS();

}
void TimeManager::BeginPlay()
{
	QueryPerformanceFrequency(&CpuFrequency);

	QueryPerformanceCounter(&PrevFrequency);
	wcout.imbue(std::locale("korean"));

	
}
void TimeManager::CalcFPS()
{
	StoreSecond += DeltaTime;
	++FrameCount;

	if (StoreSecond > 1.0f)
	{
		float FPS = FrameCount / static_cast<float>(StoreSecond);
		LastFPS = static_cast<UINT>(FPS);
		StoreSecond = 0.0f;
		FrameCount = 0;
	}
	RenderManager::GetInstance()->AddRender(1, 1, "FPS : " + to_string(LastFPS));
}
