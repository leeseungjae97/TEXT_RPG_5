#include "HUDUI.h"

#include "../Component/CombatComponent.h"
#include "../Component/EquipmentComponent.h"
#include "../Component/InventoryComponent.h"
#include "../Component/LevelComponent.h"
#include "../Define.h"
#include "../Enum/Direction.h"
#include "../Item/Item.h"
#include "../Manager/MapManager.h"
#include "../Manager/ObjectPoolManager.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/TimeManager.h"
#include "../Manager/ViewportManager.h"
#include "../Monster.h"
#include "../Player.h"
#include "../Projectile.h"

void HUDUI::Render()
{
	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

	if (!PlayerPtr)
	{
		PlayerPtr = SceneManager::GetInstance()->GetPlayer();
		return;
	}

	StatusRender();
	MapRender();
	QuickSlotRender();
}

void HUDUI::ResetCache()
{
	PlayerPtr = nullptr;
	ShopColorIndex = 0;
	ColorChangeDuration = 0.0f;
}

void HUDUI::StatusRender()
{
	const int hudY = SCREEN_HEIGHT - 13;
	const int hudX = 3;
	const int barX = hudX + 18;
	const int barWidth = 22;

	int maxHealth = max(1, PlayerPtr->GetMax_HP());
	int currentHealth = min(max(PlayerPtr->GetHP(), 0), maxHealth);
	int maxExp = max(1, PlayerPtr->GetMax_Exp());
	int currentExp = min(max(PlayerPtr->GetExp(), 0), maxExp);
	int equipmentMaxHPBonus = 0;
	int equipmentPowerBonus = 0;
	if (UEquipmentComponent* equipmentComponent = PlayerPtr->GetComponent<UEquipmentComponent>())
	{
		if (equipmentComponent->GetItem({0,0}))
			equipmentMaxHPBonus = equipmentComponent->GetItem({0,0})->GetItemInfo().EffectAmount;
		if (equipmentComponent->GetItem({1,0}))
			equipmentPowerBonus = equipmentComponent->GetItem({1,0})->GetItemInfo().EffectAmount;
	}
	if (PlayerPtr->ShouldShowLogText())
	{
		Vector isoPosition = ViewportManager::GetInstance()->GetISOPosition();
		const wstring& logText = PlayerPtr->GetLogText();
		const int textWidth = Renderer->GetTextDisplayWidth(logText);
		Renderer->AddRender(isoPosition.Y - 10, isoPosition.X - textWidth / 2, logText);
	}

	
	Renderer->AddRender(hudY, hudX, "LV : " + to_string(PlayerPtr->GetLevel()));
	Renderer->AddRender(hudY + 2, hudX, "HP : " + to_string(PlayerPtr->GetHP()) + "/" + to_string(PlayerPtr->GetMax_HP()));
	if (equipmentMaxHPBonus > 0)
	{
		Renderer->AddRender(hudY + 2, hudX + 14, L'+' + to_wstring(equipmentMaxHPBonus), CC_YELLOW);
	}
	DrawStatusBar(hudY + 2, barX, barWidth, static_cast<float>(currentHealth) / static_cast<float>(maxHealth), CC_RED);

	Renderer->AddRender(hudY + 4, hudX, "EXP : " + to_string(PlayerPtr->GetExp()) + "/" + to_string(PlayerPtr->GetMax_Exp()));
	DrawStatusBar(hudY + 4, barX, barWidth, static_cast<float>(currentExp) / static_cast<float>(maxExp), CC_YELLOW);

	Renderer->AddRender(hudY + 6, hudX, "GOLD : " + to_string(PlayerPtr->GetGold()));
	Renderer->AddRender(hudY + 8, hudX, "POWER " + to_string(PlayerPtr->GetPower()));
	if (equipmentPowerBonus > 0)
	{
		Renderer->AddRender(hudY + 8, hudX + 9 + static_cast<int>(to_string(PlayerPtr->GetPower()).length()), L'+' + to_wstring(equipmentPowerBonus), CC_YELLOW);
	}

	UCombatComponent* combatComponent = PlayerPtr->GetComponent<UCombatComponent>();
	float attackDelayRatio = 1.0f;
	if (combatComponent != nullptr && combatComponent->IsAttackCoolingDown())
	{
		attackDelayRatio = combatComponent->GetAttackCooldownAlpha();
	}

	Renderer->AddRender(hudY + 10, hudX, L"딜레이 표시 : ");
	DrawStatusBar(hudY + 10, barX, barWidth, attackDelayRatio, CC_BLUE);

	LevelComponent* levelComponent = PlayerPtr->GetComponent<LevelComponent>();
	if (levelComponent != nullptr && levelComponent->ShouldShowLevelUpText())
	{
		Renderer->AddRender(hudY - 3, hudX, L"LEVEL UP!");
		Renderer->AddRender(hudY - 2, hudX, levelComponent->GetLevelUpStateText());
	}
}

void HUDUI::MapRender()
{
	constexpr int miniMapWidth = 20;
	constexpr int miniMapHeight = 20;
	const int mapHeight = MAP_MAX_Y;
	const int mapWidth = MAP_MAX_X;
	const int boxWidth = miniMapWidth + 2;
	const int boxHeight = miniMapHeight + 2;
	const int y = 1;
	const int x = SCREEN_WIDTH - boxWidth - 3;

	Vector playerPosition = PlayerPtr->GetPosition();
	int startX = playerPosition.X - miniMapWidth / 2;
	int startY = playerPosition.Y - miniMapHeight / 2;

	startX = min(max(startX, 0), max(0, mapWidth - miniMapWidth));
	startY = min(max(startY, 0), max(0, mapHeight - miniMapHeight));

	Renderer->DrawBox(y, x, boxWidth, boxHeight);
	Renderer->AddRender(y + 1, x + max(1, (boxWidth - 6) / 2), L"미니맵");

	for (int row = 0; row < miniMapHeight; ++row)
	{
		for (int col = 0; col < miniMapWidth; ++col)
		{
			int mapY = startY + row;
			int mapX = startX + col;
			Renderer->PutCell(y + 2 + row, x + 1 + col, GetMapIcon(mapY, mapX), Renderer->MakeConsoleAttribute(GetMapIconColor(mapY, mapX)));
		}
	}
}

void HUDUI::QuickSlotRender()
{
	UInventoryComponent* inventoryComponent = PlayerPtr->GetComponent<UInventoryComponent>();
	if (inventoryComponent == nullptr)
	{
		return;
	}

	const int slotCount = 4;
	const int slotWidth = 8;
	const int slotHeight = 4;
	const int totalWidth = slotCount * (slotWidth - 1) + 1;
	const int x = SCREEN_WIDTH - totalWidth - 6;
	const int y = SCREEN_HEIGHT - slotHeight - 4;

	Renderer->AddRender(y - 2, x, "Quick Slot");

	vector<UItem*> quickSlots = inventoryComponent->GetQuickSlot();
	for (int i = 0; i < slotCount; ++i)
	{
		int slotNumber = i + 1;
		int slotX = x + i * (slotWidth - 1);
		
		UItem* item = nullptr;
		if (i < (int)quickSlots.size())
			item = quickSlots[i];

		Renderer->DrawBox(y, slotX, slotWidth, slotHeight);

		if (item == nullptr)
		{
			Renderer->PutCell(y + 2, slotX + slotWidth / 2, static_cast<wchar_t>(L'0' + slotNumber), Renderer->MakeConsoleAttribute(CC_DARKGRAY));
			continue;
		}

		Renderer->PutCell(y + 1, slotX + slotWidth / 2, GetItemIcon(item), Renderer->MakeConsoleAttribute(CC_GREEN));
		wstring name = Renderer->ToWideString(item->GetItemInfo().Name);
		name = Renderer->TrimTextToDisplayWidth(name, slotWidth - 2);
		Renderer->AddRender(y + 2, slotX + 1, name);
	}
}

void HUDUI::DrawStatusBar(int Y, int X, int Width, float Ratio, int FilledColor)
{
	Ratio = min(max(Ratio, 0.0f), 1.0f);
	int filledWidth = static_cast<int>(roundf(Ratio * Width));

	for (int i = 0; i < Width; ++i)
	{
		bool filled = i < filledWidth;
		Renderer->PutCell(Y, X + i, filled ? L' ' : L'.', Renderer->MakeConsoleAttribute(filled ? CC_BLACK : CC_DARKGRAY, filled ? FilledColor : CC_BLACK));
	}
}

wchar_t HUDUI::GetMapIcon(int MapY, int MapX)
{
	if (MapY < 0 || MapY >= MAP_MAX_Y || MapX < 0 || MapX >= MAP_MAX_X)
	{
		return L' ';
	}

	switch (MapManager::GetInstance()->GetType(MapY, MapX))
	{
	case MapObjectType::Wall:
		return L'#';
	case MapObjectType::Player:
		return L'P';
	case MapObjectType::Shop:
		return L'S';
	case MapObjectType::Monster:
		if (Monster* monster = dynamic_cast<Monster*>(MapManager::GetInstance()->GetMapObject(MapY, MapX, MapObjectType::Monster)))
		{
			string name = monster->GetName();
			if (name == "Goblin") return L'G';
			if (name == "Slime")  return L'S';
			if (name == "Orc")    return L'O';
			if (name == "Dragon") return L'D';
			if (name == "Spider") return L'A';
		}
		return L'M';
	case MapObjectType::Projectile:
		if (Projectile* projectile = dynamic_cast<Projectile*>(MapManager::GetInstance()->GetMapObject(MapY, MapX, MapObjectType::Projectile)))
		{
			switch (projectile->GetDirection())
			{
			case EDirection::UP:
				return L'^';
			case EDirection::DOWN:
				return L'v';
			case EDirection::LEFT:
				return L'<';
			case EDirection::RIGHT:
				return L'>';
			case EDirection::NONE:
			default:
				return L'*';
			}
		}
		return L'*';
	case MapObjectType::Path:
	default:
		return L'.';
	}
}

int HUDUI::GetMapIconColor(int MapY, int MapX)
{
	if (MapY < 0 || MapY >= MAP_MAX_Y || MapX < 0 || MapX >= MAP_MAX_X)
	{
		return CC_BLACK;
	}
	ColorChangeDuration += TimeManager::GetInstance()->GetDeltaTime();
	if (ColorChangeDuration >= ColorChangeInterval)
	{
		++ShopColorIndex;
		ColorChangeDuration = 0.0f;
	}
	
	switch (MapManager::GetInstance()->GetType(MapY, MapX))
	{
	case MapObjectType::Wall:
		return CC_LIGHTGRAY;
	case MapObjectType::Shop:
		return (ShopColorIndex %= CC_WHITE);
	case MapObjectType::Player:
		return CC_YELLOW;
	case MapObjectType::Monster:
		if (Monster* monster = dynamic_cast<Monster*>(MapManager::GetInstance()->GetMapObject(MapY, MapX, MapObjectType::Monster)))
		{
			if (monster->IsShiny()) return CC_RED;
		}
		return CC_WHITE;
	case MapObjectType::Projectile:
		return CC_CYAN;
	case MapObjectType::Path:
	default:
		return CC_DARKGRAY;
	}
}

wchar_t HUDUI::GetItemIcon(const UItem* Item)
{
	if (Item == nullptr)
	{
		return L' ';
	}

	switch (Item->GetItemInfo().Type)
	{
	case ItemType::Equipment:
		return L'@';
	case ItemType::Usable:
		return L'^';
	case ItemType::Misc:
		return L'*';
	default:
		return L'?';
	}
}
