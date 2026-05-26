#pragma once
#include "../Singleton.h"
#include "../UI/InventoryUI.h"
#include "../UI/HUDUI.h"
#include "../UI/ShopUI.h"
#include "../UI/DialogUI.h"
#include "../UI/BattleUI.h"

struct Vector;
class UCombatComponent;
class UMoveComponent;

class ViewportManager : public Singleton<ViewportManager>
{
public:
	ViewportManager() : PlayerPtr(nullptr), moveComponent(nullptr), combatComponent(nullptr),
	                    inventoryComponent(nullptr)
	{
	}

	~ViewportManager() {}

public:
	void Tick(float DeltaTime);
	void BeginPlay();
	void Render();
	
	void Render2Dto3D();
	void Render2DtoISO();
	void OpenInventory();
	void OpenExitDialog();
	void OpenBattleUI();
	void ShowMessageDialog(const wstring& Message, float Duration = 1.5f);

	Vector WorldToIso(float WorldX, float WorldY, int OriginX, int OriginY);
	Vector GetISOPosition();
private:
	void RenderObject();
	void RenderUI();

private:
	bool bIsInvenOpen = false;
	HUDUI PlayerStatus;
	InventoryUI Inventory;
	ShopUI Shop;
	DialogUI Dialog;
	BattleUI Battle;
	Player* PlayerPtr;
	UMoveComponent* moveComponent;
	UCombatComponent* combatComponent;
	UInventoryComponent* inventoryComponent;
};
