#pragma once
#include "../Singleton.h"
#include "../UI/InventoryUI.h"
#include "../UI/PlayerStatusUI.h"

class ViewportManager : public Singleton<ViewportManager>
{
public:
	ViewportManager() {}
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
	PlayerStatusUI PlayerStatus;
	InventoryUI Inventory;
};
