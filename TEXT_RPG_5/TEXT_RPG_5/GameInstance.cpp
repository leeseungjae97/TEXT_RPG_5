#include "GameInstance.h"
#include "Manager/RenderManager.h"
#include "Manager/BattleManager.h"
#include "Manager/MapManager.h"
#include "Manager/TimeManager.h"
#include "Manager/SceneManager.h"
#include "Manager/InputManager.h"

void GameInstance::BeginPlay()
{
	InputManager::GetInstance()->BeginPlay();
	RenderManager::GetInstance()->BeginPlay();
	TimeManager::GetInstance()->BeginPlay();

	SceneManager::GetInstance()->BeginPlay();
	MapManager::GetInstance()->BeginPlay();
	BattleManager::GetInstance()->BeginPlay();
}

void GameInstance::Tick()
{
	RenderManager::GetInstance()->ClearScreen();
	
	TimeManager::GetInstance()->Tick();
	double DeltaTime = TimeManager::GetInstance()->GetDeltaTime();
	InputManager::GetInstance()->Tick(DeltaTime);

	BattleManager::GetInstance()->Tick(DeltaTime);
	SceneManager::GetInstance()->Tick(DeltaTime);

	RenderManager::GetInstance()->Tick(DeltaTime);
	MapManager::GetInstance()->Tick(DeltaTime);
}
void GameInstance::Render()
{
	// FPS 계산
	TimeManager::GetInstance()->CalcFPS();
	UINT FPS = TimeManager::GetInstance()->LastFPS;
	RenderManager::GetInstance()->AddRender(1, 1, "FPS : " + to_string(FPS));
	RenderManager::GetInstance()->DrawScreen();
}

void GameInstance::Destroy()
{
	//TimeManager::GetInstance()->Destroy();
	SceneManager::GetInstance()->Destroy();
	//MapManager::GetInstance()->Destroy();
	//BattleManager::GetInstance()->Destroy();
	//RenderManager::GetInstance()->Destroy();
}

void GameInstance::Run()
{
	Tick();
	Render();
	Destroy();
}

GameInstance::GameInstance()
{
}

GameInstance::~GameInstance()
{
}
