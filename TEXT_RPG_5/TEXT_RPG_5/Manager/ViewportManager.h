#pragma once
#include "../Singleton.h"
#include "../UI/InventoryUI.h"
#include "../UI/HUDUI.h"

class UCombatComponent;
class UMoveComponent;

class ViewportManager : public Singleton<ViewportManager>
{
public:
	ViewportManager() : PlayerPtr(nullptr), moveComponent(nullptr), combatComponent(nullptr)
	{
	}
	~ViewportManager() {}

public:
	void Tick(float DeltaTime);
	void BeginPlay();
	void Render();
	
	void Render2Dto3D();
	void Render2DtoISO();
	
private:
	void RenderObject();
	void RenderUI();

private:
	bool bIso = false;
	bool bInven = false;
	bool bPrevInvenKey = false;
	HUDUI PlayerStatus;
	InventoryUI Inventory;
	Player* PlayerPtr;
	UMoveComponent* moveComponent;
	UCombatComponent* combatComponent;
};
