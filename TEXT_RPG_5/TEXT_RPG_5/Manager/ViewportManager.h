#pragma once
#include "../Singleton.h"
#include "../UI/InventoryUI.h"
#include "../UI/HUDUI.h"
#include "../UI/ShopUI.h"
#include "../UI/CraftingUI.h"
#include "../UI/EnhancementUI.h"
#include "../UI/DialogUI.h"
#include "../UI/BattleUI.h"
#include "../UI/ItemLogUI.h"
#include "../UI/EnterShopUI.h"
#include "../UI/ChestOpenUI.h"

struct Vector;
class UCombatComponent;
class UMoveComponent;
class UItem;

enum class ViewportFadeState
{
	None,
	FadeOut,
	FadeIn
};

enum class ViewportRenderMode
{
	ISO,
	ThreeD
};

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
	void OpenEnterShop();
	void CloseEnterShop();
	bool IsEnterShopUIOpen();
	void OpenChest();
	void CloseChest();
	bool IsChestUIOpen();
	void ShowMessageDialog(const wstring& Message, float Duration = 1.5f);
	void AddItemLog(const UItem* Item);
	void ResetRuntimeCache();
	void StartFadeOut(float Duration = 0.7f);
	void StartFadeIn(float Duration = 0.7f);
	void TickFade(float DeltaTime);
	void RenderFade();
	bool IsFadeFinished() const { return FadeState == ViewportFadeState::None; }
	bool Is3DMode() const { return RenderMode == ViewportRenderMode::ThreeD; }
	ViewportRenderMode GetRenderMode() const { return RenderMode; }
	void SetRenderMode(ViewportRenderMode InRenderMode){ RenderMode = InRenderMode;}

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
	EnterShopUI EnterShop;
	ChestOpenUI Chest;
	CraftingUI Crafting;
	EnhancementUI Enhancement;
	DialogUI Dialog;
	BattleUI Battle;
	ItemLogUI ItemLog;
	ViewportFadeState FadeState = ViewportFadeState::None;
	ViewportRenderMode RenderMode = ViewportRenderMode::ISO;
	float FadeElapsed = 0.0f;
	float FadeDuration = 0.7f;
	Player* PlayerPtr;
	UMoveComponent* moveComponent;
	UCombatComponent* combatComponent;
	UInventoryComponent* inventoryComponent;
};
