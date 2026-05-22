#include "TimeManager.h"

double TimeManager::DeltaTime = 0.0l;
double TimeManager::StoreSecond = 0.0f;
UINT TimeManager::FrameCount = 0;
UINT TimeManager::LastFPS = 0;
LARGE_INTEGER TimeManager::CpuFrequency = {};
LARGE_INTEGER TimeManager::PrevFrequency = {};
LARGE_INTEGER TimeManager::CurFrequency = {};

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

void TimeManager::Tick()
{
	if (CpuFrequency.QuadPart == 0)
	{
		Init();
	}

	QueryPerformanceCounter(&CurFrequency);

	double differnceFrequency = CurFrequency.QuadPart - PrevFrequency.QuadPart;
	DeltaTime = differnceFrequency / static_cast<double>(CpuFrequency.QuadPart);
	PrevFrequency.QuadPart = CurFrequency.QuadPart;

}
void TimeManager::Init()
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

	//wcout << L"FPS : " << LastFPS << '\n';
}
