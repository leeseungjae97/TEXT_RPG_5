#include "GameInstance.h"
#include "Manager/DisplayManager.h"
#include "Manager/BattleManager.h"
#include "Manager/ViewportManager.h"
#include "Manager/MapManager.h"
#include "Manager/TimeManager.h"
#include "Manager/SceneManager.h"
#include "Manager/InputManager.h"
#include "Manager/ObjectPoolManager.h"
#include "Manager/ShopManager.h"

void GameInstance::BeginPlay()
{
	InputManager::GetInstance()->BeginPlay();
	DisplayManager::GetInstance()->BeginPlay();
	TimeManager::GetInstance()->BeginPlay();
	ShopManager::GetInstance()->BeginPlay();
	MapManager::GetInstance()->BeginPlay();
	SceneManager::GetInstance()->BeginPlay();
	BattleManager::GetInstance()->BeginPlay();
}

void GameInstance::Tick()
{
	TimeManager::GetInstance()->Tick();
	double DeltaTime = TimeManager::GetInstance()->GetDeltaTime();
	
	InputManager::GetInstance()->Tick(DeltaTime);

	SceneManager::GetInstance()->Tick(DeltaTime);

	ViewportManager::GetInstance()->Tick(DeltaTime);
}
void GameInstance::Render()
{
	ViewportManager::GetInstance()->Render();
	DisplayManager::GetInstance()->Render();
}

void GameInstance::Destroy()
{
	SceneManager::GetInstance()->Destroy();
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
