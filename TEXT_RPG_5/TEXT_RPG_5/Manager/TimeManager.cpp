#include "TimeManager.h"

#include "DisplayManager.h"

TimeManager::TimeManager()
	: DeltaTime(0.f), StoreSecond(0.0f), StoredTime(0.0f), FrameCount(0), LastFPS(0),
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
	DisplayManager::GetInstance()->AddRender(1, 1, "FPS : " + to_string(LastFPS));
}

wstring TimeManager::FormatSeconds(double Seconds)
{
	Seconds = max(0.0, Seconds);
	const int hours = static_cast<int>(Seconds / 3600.0);
	Seconds -= static_cast<double>(hours * 3600);
	const int minutes = static_cast<int>(Seconds / 60.0);
	Seconds -= static_cast<double>(minutes * 60);
	const int seconds = static_cast<int>(Seconds);
	const int milliseconds = static_cast<int>((Seconds - static_cast<double>(seconds)) * 1000.0);

	wstringstream stream;
	stream << setfill(L'0')
		<< setw(2) << hours << L":"
		<< setw(2) << minutes << L":"
		<< setw(2) << seconds << L"."
		<< setw(3) << milliseconds;

	return stream.str();
}
