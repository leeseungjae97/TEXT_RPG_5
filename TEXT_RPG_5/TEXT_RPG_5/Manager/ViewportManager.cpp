#include "ViewportManager.h"

#include "InputManager.h"
#include "MapManager.h"
#include "DisplayManager.h"
#include "SceneManager.h"
#include "StageManager.h"
#include "../Component/CombatComponent.h"
#include "../Component/EffectComponent.h"
#include "../Component/EquipmentComponent.h"
#include "../Component/InventoryComponent.h"
#include "../Component/LevelComponent.h"
#include "../Component/MoveComponent.h"
#include "../Define.h"
#include "../Monster.h"
#include "../Player.h"
#include "../Projectile.h"

namespace
{
	constexpr float PI = 3.14159f;

	float Clamp01(float Value)
	{
		return min(max(Value, 0.0f), 1.0f);
	}

	enum class WallFace
	{
		None,
		Left,
		Right,
		Up,
		Down
	};

	WORD MakeAttribute(int Color, int BackgroundColor = CC_BLACK)
	{
		return static_cast<WORD>(((BackgroundColor & 0xf) << 4) | (Color & 0xf));
	}

	FVector InterpolatePosition(const Vector& PrevPosition, const Vector& CurrentPosition, float Alpha)
	{
		Alpha = min(max(Alpha, 0.0f), 1.0f);

		return {
			static_cast<float>(PrevPosition.X) + static_cast<float>(CurrentPosition.X - PrevPosition.X) * Alpha + 0.5f,
			static_cast<float>(PrevPosition.Y) + static_cast<float>(CurrentPosition.Y - PrevPosition.Y) * Alpha + 0.5f
		};
	}

	Vector GetRenderTargetPosition(AObject* Object)
	{
		if (Object == nullptr)
		{
			return {0, 0};
		}

		return Object->IsMoving() ? Object->GetNextPosition() : Object->GetPosition();
	}

	float NormalizeAngle(float Angle)
	{
		while (Angle < -PI) Angle += 2.0f * PI;
		while (Angle > PI) Angle -= 2.0f * PI;

		return Angle;
	}

	float DirectionToAngle(EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::UP:
			return PI;
		case EDirection::DOWN:
			return 0.0f;
		case EDirection::LEFT:
			return -PI / 2.0f;
		case EDirection::RIGHT:
			return PI / 2.0f;
		case EDirection::NONE:
		default:
			return PI;
		}
	}

	float InterpolateAngle(float PrevAngle, float CurrentAngle, float Alpha)
	{
		Alpha = min(max(Alpha, 0.0f), 1.0f);

		const float DeltaAngle = NormalizeAngle(CurrentAngle - PrevAngle);
		return NormalizeAngle(PrevAngle + DeltaAngle * Alpha);
	}

	WORD GetWallAttribute(WallFace Face, float Distance, float Depth)
	{
		const bool NearWall = Distance < Depth * 0.55f;

		switch (Face)
		{
		case WallFace::Left:
			return MakeAttribute(NearWall ? CC_RED : CC_DARKRED);
		case WallFace::Right:
			return MakeAttribute(NearWall ? CC_CYAN : CC_DARKCYAN);
		case WallFace::Up:
			return MakeAttribute(NearWall ? CC_GREEN : CC_DARKGREEN);
		case WallFace::Down:
			return MakeAttribute(NearWall ? CC_YELLOW : CC_DARKYELLOW);
		default:
			return MakeAttribute(CC_GRAY);
		}
	}

	wchar_t GetWallCharacter(float Distance, float Depth, bool Edge)
	{
		if (Edge)
		{
			return 0x2591;
		}

		if (Distance <= Depth / 5.0f)      return 0x2588;
		if (Distance <= Depth / 3.0f)      return 0x2593;
		if (Distance <= Depth / 2.0f)      return 0x2592;
		if (Distance < Depth)              return 0x2591;

		return L' ';
	}

	wchar_t GetMonsterIcon(Monster* MonsterPtr)
	{
		if (MonsterPtr == nullptr)
		{
			return L'M';
		}

		const wstring& name = MonsterPtr->GetDisplayName();
		if (name.find(L"고블린") != wstring::npos) return L'G';
		if (name.find(L"슬라임") != wstring::npos) return L'S';
		if (name.find(L"오크") != wstring::npos) return L'O';
		if (name.find(L"드래곤") != wstring::npos) return L'D';
		if (name.find(L"거미") != wstring::npos) return L'A';
		return L'M';
	}

	int GetMonsterColor(Monster* MonsterPtr)
	{
		if (MonsterPtr == nullptr)
		{
			return CC_MAGENTA;
		}

		if (MonsterPtr->IsShiny()) return CC_RED;

		const wstring& name = MonsterPtr->GetDisplayName();
		if (name.find(L"고블린") != wstring::npos) return CC_GREEN;
		if (name.find(L"슬라임") != wstring::npos) return CC_CYAN;
		if (name.find(L"오크") != wstring::npos) return CC_DARKYELLOW;
		if (name.find(L"드래곤") != wstring::npos) return CC_RED;
		if (name.find(L"거미") != wstring::npos) return CC_DARKYELLOW;
		return CC_MAGENTA;
	}
	wchar_t GetDirectionArrow(EDirection Direction)
	{
		switch (Direction)
		{
		case EDirection::UP:
			return L'↗';
		case EDirection::DOWN:
			return L'↙';
		case EDirection::LEFT:
			return L'↖';
		case EDirection::RIGHT:
			return L'↘';
		case EDirection::NONE:
		default:
			return L'*';
		}
	}
}

void ViewportManager::Render2DtoISO()
{
	DisplayManager* renderManager = DisplayManager::GetInstance();

	const int originX = SCREEN_WIDTH / 2;
	const int originY = SCREEN_HEIGHT / 3;
	constexpr int viewRadiusX = 8;
	constexpr int viewRadiusY = 8;

	Player* currentPlayer = SceneManager::GetInstance()->GetPlayer();
	FVector cameraPosition = {
		static_cast<float>(MAP_MAX_X) / 2.0f,
		static_cast<float>(MAP_MAX_Y) / 2.0f
	};

	if (currentPlayer != nullptr)
	{
		float moveAlpha = 1.0f;
		if (UMoveComponent* moveComponent = currentPlayer->GetComponent<UMoveComponent>())
		{
			moveAlpha = moveComponent->GetMoveAlpha();
		}

		cameraPosition = InterpolatePosition(currentPlayer->GetPrevPosition(), GetRenderTargetPosition(currentPlayer), moveAlpha);
		cameraPosition.X -= 0.5f;
		cameraPosition.Y -= 0.5f;
	}

	auto drawIsoDiamond = [&](Vector iso, WORD attribute)
		{
			constexpr int tileHalfWidth = 8;
			constexpr int tileHalfHeight = 2;
			constexpr int stepX = tileHalfWidth / tileHalfHeight;

			for (int i = 0; i < tileHalfHeight; ++i)
			{
				renderManager->PutCell(iso.Y - tileHalfHeight + i, iso.X - i * stepX, L'/', attribute);
				renderManager->PutCell(iso.Y - tileHalfHeight + i, iso.X + i * stepX, L'\\', attribute);
				renderManager->PutCell(iso.Y + i, iso.X - tileHalfWidth + i * stepX, L'\\', attribute);
				renderManager->PutCell(iso.Y + i, iso.X + tileHalfWidth - i * stepX, L'/', attribute);
			}
		};

	auto drawIsoWall = [&](Vector iso)
		{
			constexpr int wallHeight = 6;
			constexpr int tileHalfWidth = 8;
			WORD topAttribute = MakeAttribute(CC_LIGHTGRAY);
			WORD sideAttribute = MakeAttribute(CC_GRAY);
			WORD darkSideAttribute = MakeAttribute(CC_DARKGRAY);

			Vector topIso = { iso.X, iso.Y - wallHeight };

			drawIsoDiamond(topIso, topAttribute);

			for (int y = topIso.Y; y <= iso.Y; ++y)
			{
				renderManager->PutCell(y, iso.X - tileHalfWidth, L'|', darkSideAttribute);
				renderManager->PutCell(y, iso.X + tileHalfWidth, L'|', sideAttribute);
			}

			renderManager->PutCell(topIso.Y + 1, iso.X - 4, L'#', topAttribute);
			renderManager->PutCell(topIso.Y + 1, iso.X, L'#', topAttribute);
			renderManager->PutCell(topIso.Y + 1, iso.X + 4, L'#', topAttribute);

			renderManager->PutCell(iso.Y - 4, iso.X - 7, L':', darkSideAttribute);
			renderManager->PutCell(iso.Y - 3, iso.X - 7, L':', darkSideAttribute);
			renderManager->PutCell(iso.Y - 2, iso.X + 7, L':', sideAttribute);
			renderManager->PutCell(iso.Y - 1, iso.X + 7, L':', sideAttribute);
		};

	auto drawIsoShop = [&](Vector iso)
		{
			WORD woodAttribute = MakeAttribute(CC_DARKYELLOW);
			WORD darkWoodAttribute = MakeAttribute(CC_DARKRED);
			WORD clothRedAttribute = MakeAttribute(CC_RED);
			WORD clothCreamAttribute = MakeAttribute(CC_YELLOW);
			WORD counterAttribute = MakeAttribute(CC_CYAN);
			WORD itemAttribute = MakeAttribute(CC_GREEN);
			WORD shadowAttribute = MakeAttribute(CC_DARKGRAY);
		
			for (int dx = -4; dx <= 4; ++dx)
			{
				renderManager->PutCell(iso.Y + 1, iso.X + dx, L'_', shadowAttribute);
			}

			for (int y = iso.Y - 6; y <= iso.Y - 1; ++y)
			{
				renderManager->PutCell(y, iso.X - 5, L'|', woodAttribute);
				renderManager->PutCell(y, iso.X + 5, L'|', woodAttribute);
			}
		
			wstring ShopStr = L"Shop";
			for (int dx = -2; dx <= 2; ++dx)
			{
				renderManager->PutCell(iso.Y - 7, iso.X + dx, ShopStr[dx + 2], shadowAttribute);
			}
			for (int dx = -5; dx <= 5; ++dx)
			{
				WORD clothAttribute = ((dx + 6) / 2) % 2 == 0 ? clothRedAttribute : clothCreamAttribute;
				renderManager->PutCell(iso.Y - 6, iso.X + dx, L'_', clothAttribute);
				renderManager->PutCell(iso.Y - 5, iso.X + dx, L'=', clothAttribute);
				if (dx % 3 == 0)
				{
					renderManager->PutCell(iso.Y - 4, iso.X + dx, L'v', clothAttribute);
				}
			}

			renderManager->PutCell(iso.Y - 5, iso.X - 6, L'/', woodAttribute);
			renderManager->PutCell(iso.Y - 5, iso.X + 6, L'\\', woodAttribute);
			renderManager->PutCell(iso.Y - 4, iso.X - 4, L'~', clothCreamAttribute);
			renderManager->PutCell(iso.Y - 4, iso.X, L'~', clothRedAttribute);
			renderManager->PutCell(iso.Y - 4, iso.X + 4, L'~', clothCreamAttribute);

			for (int dx = -4; dx <= 4; ++dx)
			{
				renderManager->PutCell(iso.Y - 2, iso.X + dx, L'=', counterAttribute);
			}

			for (int dx = -4; dx <= 4; dx += 4)
			{
				renderManager->PutCell(iso.Y - 1, iso.X + dx, L'V', woodAttribute);
			}

			renderManager->PutCell(iso.Y - 3, iso.X - 2, L'!', clothRedAttribute);
			renderManager->PutCell(iso.Y - 3, iso.X, L'o', itemAttribute);
			renderManager->PutCell(iso.Y - 3, iso.X + 2, L'#', clothCreamAttribute);

			renderManager->PutCell(iso.Y, iso.X - 5, L'/', darkWoodAttribute);
			renderManager->PutCell(iso.Y, iso.X + 5, L'\\', darkWoodAttribute);
			renderManager->PutCell(iso.Y, iso.X + 6, L'o', woodAttribute);
		};

	auto drawIsoCrystal = [&](Vector iso)
		{
			WORD crystalAttribute = MakeAttribute(StageManager::GetInstance()->GetCrystalColor());
			WORD shineAttribute = MakeAttribute(CC_WHITE);
			renderManager->PutCell(iso.Y - 5, iso.X, L'*', shineAttribute);
			renderManager->PutCell(iso.Y - 4, iso.X - 1, L'/', crystalAttribute);
			renderManager->PutCell(iso.Y - 4, iso.X, L'C', crystalAttribute);
			renderManager->PutCell(iso.Y - 4, iso.X + 1, L'\\', crystalAttribute);
			renderManager->PutCell(iso.Y - 3, iso.X - 2, L'<', crystalAttribute);
			renderManager->PutCell(iso.Y - 3, iso.X, L'|', shineAttribute);
			renderManager->PutCell(iso.Y - 3, iso.X + 2, L'>', crystalAttribute);
			renderManager->PutCell(iso.Y - 2, iso.X - 1, L'\\', crystalAttribute);
			renderManager->PutCell(iso.Y - 2, iso.X, L'V', crystalAttribute);
			renderManager->PutCell(iso.Y - 2, iso.X + 1, L'/', crystalAttribute);
		};

	auto drawIsoActor = [&](Vector iso, wchar_t icon, WORD attribute, bool player)
		{
			WORD shadowAttribute = MakeAttribute(CC_DARKGRAY);
			renderManager->PutCell(iso.Y, iso.X - 2, L'(', shadowAttribute);
			renderManager->PutCell(iso.Y, iso.X - 1, L'_', shadowAttribute);
			renderManager->PutCell(iso.Y, iso.X, L'_', shadowAttribute);
			renderManager->PutCell(iso.Y, iso.X + 1, L')', shadowAttribute);

			renderManager->PutCell(iso.Y - 4, iso.X, icon, attribute);
			renderManager->PutCell(iso.Y - 3, iso.X - 1, player ? L'/' : L'{', attribute);
			renderManager->PutCell(iso.Y - 3, iso.X, player ? L'|' : L'M', attribute);
			renderManager->PutCell(iso.Y - 3, iso.X + 1, player ? L'\\' : L'}', attribute);
			renderManager->PutCell(iso.Y - 2, iso.X - 1, L'/', attribute);
			renderManager->PutCell(iso.Y - 2, iso.X, L'|', attribute);
			renderManager->PutCell(iso.Y - 2, iso.X + 1, L'\\', attribute);
			renderManager->PutCell(iso.Y - 1, iso.X - 1, L'/', attribute);
			renderManager->PutCell(iso.Y - 1, iso.X + 1, L'\\', attribute);
		};

	auto drawPlayerWeapon = [&](Vector iso, WeaponType weaponType, EDirection direction, float attackAnimationAlpha, bool bAttackAnimating)
		{
			if (weaponType == WeaponType::NONE)
			{
				return;
			}

			const int handSide = direction == EDirection::DOWN || direction == EDirection::LEFT ? -1 : 1;
			int handX = iso.X + 2;
			int handY = iso.Y - 3;
			switch (direction)
			{
			case EDirection::DOWN:
				handX = iso.X - 2;
				handY = iso.Y - 3;
				break;
			case EDirection::LEFT:
				handX = iso.X - 2;
				handY = iso.Y - 2;
				break;
			case EDirection::RIGHT:
				handX = iso.X + 2;
				handY = iso.Y - 2;
				break;
			case EDirection::UP:
			case EDirection::NONE:
			default:
				break;
			}

			WORD weaponAttribute = MakeAttribute(CC_WHITE);
			wchar_t weaponChar = L'|';
			const float returnAlpha = attackAnimationAlpha < 0.5f
				? attackAnimationAlpha * 2.0f
				: (1.0f - attackAnimationAlpha) * 2.0f;

			if (weaponType == WeaponType::Sword || weaponType == WeaponType::Axe)
			{
				weaponAttribute = MakeAttribute(weaponType == WeaponType::Axe ? CC_DARKGRAY : CC_LIGHTGRAY);
				if (bAttackAnimating)
				{
					if (attackAnimationAlpha < 0.34f)
					{
						weaponChar = handSide < 0 ? L'\\' : L'/';
						handY -= 1;
					}
					else if (attackAnimationAlpha < 0.67f)
					{
						weaponChar = weaponType == WeaponType::Axe ? L'T' : L'-';
						handX += handSide;
					}
					else
					{
						weaponChar = handSide < 0 ? L'/' : L'\\';
						handY += 1;
					}
				}
				else
				{
					weaponChar = weaponType == WeaponType::Axe
						? L'P'
						: (handSide < 0 ? L'/' : L'\\');
				}
			}
			else if (weaponType == WeaponType::Bow)
			{
				weaponAttribute = MakeAttribute(CC_YELLOW);
				weaponChar = handX < iso.X ? L'(' : L')';
				const int pullOffset = bAttackAnimating && returnAlpha > 0.35f ? -handSide : 0;
				renderManager->PutCell(handY, handX, weaponChar, weaponAttribute);
				renderManager->PutCell(handY, handX - handSide + pullOffset, returnAlpha > 0.35f ? L'<' : L'|', MakeAttribute(CC_LIGHTGRAY));
				if (bAttackAnimating && returnAlpha > 0.65f)
				{
					renderManager->PutCell(handY, handX + handSide, handSide > 0 ? L'>' : L'<', MakeAttribute(CC_CYAN));
				}
				return;
			}
			else if (weaponType == WeaponType::Magic)
			{
				weaponAttribute = MakeAttribute(CC_MAGENTA);
				const int lift = bAttackAnimating ? static_cast<int>(returnAlpha * 2.0f + 0.5f) : 0;
				weaponChar = L'|';
				handY -= lift;
				renderManager->PutCell(handY - 1, handX, L'*', MakeAttribute(CC_MAGENTA));
			}

			renderManager->PutCell(handY, handX, weaponChar, weaponAttribute);
		};

	auto drawIsoPlayerActor = [&](Vector iso, wchar_t icon, WORD attribute, Player* player)
		{
			drawIsoActor(iso, icon, attribute, true);

			if (player == nullptr)
			{
				return;
			}

			WeaponType weaponType = WeaponType::NONE;
			if (UEquipmentComponent* equipmentComponent = player->GetComponent<UEquipmentComponent>())
			{
				weaponType = equipmentComponent->GetCurrentWeaponType();
			}

			EDirection direction = EDirection::UP;
			if (UMoveComponent* moveComponent = player->GetComponent<UMoveComponent>())
			{
				direction = moveComponent->GetFacingDirection();
			}

			float attackAnimationAlpha = 1.0f;
			bool bAttackAnimating = false;
			if (UCombatComponent* combatComponent = player->GetComponent<UCombatComponent>())
			{
				bAttackAnimating = combatComponent->IsAttackVisible();
				attackAnimationAlpha = combatComponent->GetAttackAnimationAlpha();
			}

			drawPlayerWeapon(iso, weaponType, direction, attackAnimationAlpha, bAttackAnimating);
		};

	auto drawIsoMonsterActor = [&](Vector iso, Monster* monster, WORD attribute)
		{
			if (monster == nullptr)
			{
				return;
			}

			WORD shadowAttribute = MakeAttribute(CC_DARKGRAY);
			renderManager->PutCell(iso.Y, iso.X - 2, L'(', shadowAttribute);
			renderManager->PutCell(iso.Y, iso.X - 1, L'_', shadowAttribute);
			renderManager->PutCell(iso.Y, iso.X, L'_', shadowAttribute);
			renderManager->PutCell(iso.Y, iso.X + 1, L')', shadowAttribute);

			const wstring& name = monster->GetDisplayName();
			if (name.find(L"고블린") != wstring::npos)
			{
				renderManager->PutCell(iso.Y - 4, iso.X - 2, L'<', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X, L'G', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X + 2, L'>', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X - 1, L'/', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X, L'|', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 1, L'\\', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 2, L'/', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 2, L'|', MakeAttribute(CC_DARKGRAY));
				renderManager->PutCell(iso.Y - 1, iso.X - 1, L'/', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 1, L'\\', attribute);
				return;
			}

			if (name.find(L"슬라임") != wstring::npos)
			{
				renderManager->PutCell(iso.Y - 3, iso.X - 2, L'_', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X - 1, L'_', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X, L'S', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 1, L'_', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 2, L'_', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 3, L'/', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 1, L'o', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 1, L'o', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 3, L'\\', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 2, L'\\', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 1, L'_', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X, L'_', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 1, L'_', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 2, L'/', attribute);
				return;
			}

			if (name.find(L"오크") != wstring::npos)
			{
				renderManager->PutCell(iso.Y - 5, iso.X, L'O', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X - 2, L'T', MakeAttribute(CC_DARKGRAY));
				renderManager->PutCell(iso.Y - 4, iso.X - 1, L'/', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X, L'|', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X + 1, L'\\', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X - 1, L'/', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X, L'|', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 1, L'\\', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 1, L'|', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 1, L'|', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 1, L'/', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 1, L'\\', attribute);
				return;
			}

			if (name.find(L"드래곤") != wstring::npos)
			{
				WORD wingAttribute = MakeAttribute(monster->IsShiny() ? CC_RED : CC_DARKRED);
				WORD boneAttribute = MakeAttribute(CC_YELLOW);
				renderManager->PutCell(iso.Y - 6, iso.X - 5, L'/', wingAttribute);
				renderManager->PutCell(iso.Y - 6, iso.X + 5, L'\\', wingAttribute);
				renderManager->PutCell(iso.Y - 5, iso.X - 4, L'/', wingAttribute);
				renderManager->PutCell(iso.Y - 5, iso.X - 2, L'_', wingAttribute);
				renderManager->PutCell(iso.Y - 5, iso.X + 2, L'_', wingAttribute);
				renderManager->PutCell(iso.Y - 5, iso.X + 4, L'\\', wingAttribute);
				renderManager->PutCell(iso.Y - 4, iso.X - 3, L'\\', wingAttribute);
				renderManager->PutCell(iso.Y - 4, iso.X - 1, L'D', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X, L'R', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X + 1, L'>', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X - 2, L'/', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X, L'=', boneAttribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 2, L'\\', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 3, L'~', wingAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 1, L'/', attribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 1, L'\\', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 4, L'~', wingAttribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 2, L'/', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 2, L'\\', attribute);
				return;
			}

			if (name.find(L"거미") != wstring::npos)
			{
				WORD legAttribute = MakeAttribute(CC_DARKYELLOW);
				WORD eyeAttribute = MakeAttribute(CC_RED);
				renderManager->PutCell(iso.Y - 4, iso.X - 3, L'\\', legAttribute);
				renderManager->PutCell(iso.Y - 4, iso.X + 3, L'/', legAttribute);
				renderManager->PutCell(iso.Y - 3, iso.X - 4, L'\\', legAttribute);
				renderManager->PutCell(iso.Y - 3, iso.X - 1, L'(', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X, L'o', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 1, L')', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 4, L'/', legAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 5, L'/', legAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 2, L'/', legAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 1, L'x', eyeAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X, L'x', eyeAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 1, L'\\', legAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 5, L'\\', legAttribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 4, L'\\', legAttribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 2, L'|', legAttribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 2, L'|', legAttribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 4, L'/', legAttribute);
				return;
			}

			drawIsoActor(iso, L'M', attribute, false);
		};

	auto drawStatusBar = [&](int y, int centerX, float ratio, int filledColor)
		{
			constexpr int barWidth = 8;
			ratio = min(max(ratio, 0.0f), 1.0f);
			int filledWidth = static_cast<int>(roundf(ratio * barWidth));

			int startX = centerX - barWidth / 2 - 1;

			renderManager->PutCell(y, startX, L'[', MakeAttribute(CC_GRAY));
			for (int i = 0; i < barWidth; ++i)
			{
				bool filled = i < filledWidth;
				renderManager->PutCell(y, startX + 1 + i, filled ? L'=' : L'-', MakeAttribute(filled ? filledColor : CC_DARKGRAY));
			}
			renderManager->PutCell(y, startX + barWidth + 1, L']', MakeAttribute(CC_GRAY));
		};

	auto drawMonsterHpBar = [&](Vector iso, Monster* monster)
		{
			if (monster == nullptr)
			{
				return;
			}

			int maxHealth = max(1, monster->GetMaxHealth());
			int currentHealth = min(max(monster->GetHealth(), 0), maxHealth);
			drawStatusBar(iso.Y - 6, iso.X, static_cast<float>(currentHealth) / static_cast<float>(maxHealth), CC_RED);
		};

	auto drawMonsterNameLabel = [&](Vector iso, Monster* monster)
		{
			if (monster == nullptr)
			{
				return;
			}

			wstring label = L"LV." + to_wstring(monster->GetLevel()) + L" " + monster->GetDisplayName();
			const int textWidth = renderManager->GetTextDisplayWidth(label);
			const int startX = iso.X - textWidth / 2;
			const int y = iso.Y - 8;

			const int labelColor = monster->IsShiny() ? CC_RED : CC_WHITE;
			renderManager->AddRender(y, startX, label, labelColor);
		};

	auto drawObjectHitEffect = [&](Vector iso, AObject* object)
		{
			if (object == nullptr || !object->ShouldShowHitEffect())
			{
				return;
			}

			if (object->GetHitEffectType() == HitEffectType::Bow)
			{
				WORD attribute = MakeAttribute(CC_CYAN);
				renderManager->PutCell(iso.Y - 5, iso.X - 2, L'X', attribute);
				renderManager->PutCell(iso.Y - 5, iso.X + 2, L'X', attribute);
				renderManager->PutCell(iso.Y - 4, iso.X, L'X', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X - 2, L'X', attribute);
				renderManager->PutCell(iso.Y - 3, iso.X + 2, L'X', attribute);
				return;
			}

			if (object->GetHitEffectType() == HitEffectType::Magic)
			{
				WORD fireAttribute = MakeAttribute(CC_RED);
				WORD hotAttribute = MakeAttribute(CC_YELLOW);
				renderManager->PutCell(iso.Y - 1, iso.X - 2, L'/', fireAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X - 1, L'^', hotAttribute);
				renderManager->PutCell(iso.Y - 3, iso.X, L'W', hotAttribute);
				renderManager->PutCell(iso.Y - 2, iso.X + 1, L'^', hotAttribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 2, L'\\', fireAttribute);
				renderManager->PutCell(iso.Y - 4, iso.X, L'|', fireAttribute);
			}
		};

	auto drawPlayerBars = [&](Vector iso, Player* player)
		{
			if (player == nullptr)
			{
				return;
			}

			int maxHealth = max(1, player->GetMax_HP());
			int currentHealth = min(max(player->GetHP(), 0), maxHealth);
			int maxExp = max(1, player->GetMax_Exp());
			int currentExp = min(max(player->GetExp(), 0), maxExp);

			drawStatusBar(iso.Y - 8, iso.X, static_cast<float>(currentExp) / static_cast<float>(maxExp), CC_GREEN);
			drawStatusBar(iso.Y - 7, iso.X, static_cast<float>(currentHealth) / static_cast<float>(maxHealth), CC_RED);

			UCombatComponent* combatComponent = player->GetComponent<UCombatComponent>();
			if (combatComponent != nullptr && combatComponent->IsAttackCoolingDown())
			{
				drawStatusBar(iso.Y - 6, iso.X, combatComponent->GetAttackCooldownAlpha(), CC_CYAN);
			}
		};

	auto drawLevelUpEffect = [&](Vector iso, Player* player)
		{
			if (player == nullptr)
			{
				return;
			}

			LevelComponent* levelComponent = player->GetComponent<LevelComponent>();
			if (levelComponent == nullptr || !levelComponent->ShouldShowLevelUpText())
			{
				return;
			}

			renderManager->AddRender(iso.Y - 11, iso.X - 4, L"LEVEL UP!");
			renderManager->AddRender(iso.Y - 10, iso.X - static_cast<int>(levelComponent->GetLevelUpStateText().length()) / 2, levelComponent->GetLevelUpStateText());

			float effectAlpha = levelComponent->GetLevelUpEffectAlpha();
			int radiusX = 5 + static_cast<int>(roundf(effectAlpha * 5.0f));
			int radiusY = max(2, radiusX / 3);
			WORD effectAttribute = MakeAttribute(effectAlpha < 0.5f ? CC_YELLOW : CC_CYAN);

			for (int i = 0; i < 16; ++i)
			{
				float angle = 2.0f * PI * static_cast<float>(i) / 16.0f + effectAlpha * 2.0f * PI;
				int x = iso.X + static_cast<int>(roundf(cosf(angle) * radiusX));
				int y = iso.Y - 2 + static_cast<int>(roundf(sinf(angle) * radiusY));
				renderManager->PutCell(y, x, L'o', effectAttribute);
			}
		};

	auto drawItemUseEffect = [&](Vector iso, Player* player)
		{
			if (player == nullptr)
			{
				return;
			}

			UEffectComponent* effectComponent = player->GetComponent<UEffectComponent>();
			if (effectComponent == nullptr || !effectComponent->ShouldShowItemUseEffect())
			{
				return;
			}

			const float alpha = effectComponent->GetItemUseEffectAlpha();
			const int rise = static_cast<int>(roundf(alpha * 3.0f));

			if (effectComponent->GetItemUseEffectType() == EItemUseEffectType::Buff)
			{
				WORD auraAttribute = MakeAttribute(alpha < 0.5f ? CC_YELLOW : CC_MAGENTA);
				const int radiusX = 4 + static_cast<int>(roundf(alpha * 3.0f));
				const int radiusY = 1 + static_cast<int>(roundf(alpha * 2.0f));
				for (int i = 0; i < 12; ++i)
				{
					float angle = 2.0f * PI * static_cast<float>(i) / 12.0f + alpha * PI;
					int x = iso.X + static_cast<int>(roundf(cosf(angle) * radiusX));
					int y = iso.Y - 2 + static_cast<int>(roundf(sinf(angle) * radiusY));
					renderManager->PutCell(y, x, L'*', auraAttribute);
				}
				renderManager->PutCell(iso.Y - 5 - rise, iso.X, L'^', auraAttribute);
				return;
			}

			WORD consumeAttribute = MakeAttribute(alpha < 0.5f ? CC_GREEN : CC_WHITE);
			renderManager->PutCell(iso.Y - 5 - rise, iso.X, L'+', consumeAttribute);
			renderManager->PutCell(iso.Y - 4 - rise, iso.X - 1, L'+', consumeAttribute);
			renderManager->PutCell(iso.Y - 4 - rise, iso.X + 1, L'+', consumeAttribute);
		};

	auto drawTeleportEffects = [&](UCombatComponent* combatComponent)
		{
			if (combatComponent == nullptr)
			{
				return;
			}

			for (const FTeleportEffect& effect : combatComponent->GetTeleportEffects())
			{
				if (fabsf(static_cast<float>(effect.Position.X) - 0.5f - cameraPosition.X) > viewRadiusX ||
					fabsf(static_cast<float>(effect.Position.Y) - 0.5f - cameraPosition.Y) > viewRadiusY)
				{
					continue;
				}

				Vector iso = WorldToIso(
					static_cast<float>(effect.Position.X) - 0.5f - cameraPosition.X,
					static_cast<float>(effect.Position.Y) - 0.5f - cameraPosition.Y,
					originX,
					originY
				);

				const float alpha = effect.Duration <= 0.0f ? 1.0f : effect.Elapsed / effect.Duration;
				WORD attribute = MakeAttribute(alpha < 0.5f ? CC_MAGENTA : CC_CYAN);
				renderManager->PutCell(iso.Y - 2, iso.X, L'*', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X - 2, L'/', attribute);
				renderManager->PutCell(iso.Y - 1, iso.X + 2, L'\\', attribute);
				renderManager->PutCell(iso.Y, iso.X - 4, L'<', attribute);
				renderManager->PutCell(iso.Y, iso.X + 4, L'>', attribute);
			}
		};

	auto drawIsoTile = [&](int mapX, int mapY, bool wall)
		{
			Vector iso = WorldToIso(
				static_cast<float>(mapX) - cameraPosition.X,
				static_cast<float>(mapY) - cameraPosition.Y,
				originX,
				originY
			);

			if (wall)
			{
				drawIsoDiamond(iso, MakeAttribute(CC_GRAY));
				drawIsoWall(iso);
				return;
			}

			WORD floorAttribute = ((mapX + mapY) % 2 == 0)
				? MakeAttribute(CC_DARKGRAY)
				: MakeAttribute(CC_GRAY);

			drawIsoDiamond(iso, floorAttribute);
		};

	auto drawAttackPositions = [&]()
		{
			auto drawSlashTile = [&](Vector iso, int color, WeaponType weaponType)
				{
					WORD attribute = MakeAttribute(color);
					if (weaponType == WeaponType::Axe)
					{
						renderManager->PutCell(iso.Y - 1, iso.X - 1, L'\\', attribute);
						renderManager->PutCell(iso.Y, iso.X, L'=', attribute);
						renderManager->PutCell(iso.Y + 1, iso.X + 1, L'/', attribute);
						return;
					}

					renderManager->PutCell(iso.Y - 1, iso.X + 1, L'/', attribute);
					renderManager->PutCell(iso.Y, iso.X, L'-', attribute);
					renderManager->PutCell(iso.Y + 1, iso.X - 1, L'\\', attribute);
				};

			auto drawFireTile = [&](Vector iso, bool centerFire)
				{
					WORD baseAttribute = MakeAttribute(CC_RED);
					WORD hotAttribute = MakeAttribute(CC_YELLOW);
					renderManager->PutCell(iso.Y, iso.X, L'^', hotAttribute);
					renderManager->PutCell(iso.Y - 1, iso.X - 1, L'/', baseAttribute);
					renderManager->PutCell(iso.Y - 1, iso.X, centerFire ? L'W' : L'v', hotAttribute);
					renderManager->PutCell(iso.Y - 1, iso.X + 1, L'\\', baseAttribute);
					if (centerFire)
					{
						renderManager->PutCell(iso.Y - 2, iso.X, L'^', hotAttribute);
						renderManager->PutCell(iso.Y - 3, iso.X, L'|', baseAttribute);
					}
				};

			auto drawDefaultAttackTile = [&](Vector iso, int color)
				{
					WORD attribute = MakeAttribute(color);
					renderManager->PutCell(iso.Y - 1, iso.X, L'*', attribute);
					renderManager->PutCell(iso.Y, iso.X - 1, L'<', attribute);
					renderManager->PutCell(iso.Y, iso.X, L'X', attribute);
					renderManager->PutCell(iso.Y, iso.X + 1, L'>', attribute);
				};

			auto drawAttackTile = [&](const Vector& attackPosition, int color, WeaponType weaponType)
				{
					if (fabsf(static_cast<float>(attackPosition.X) - cameraPosition.X) > viewRadiusX ||
						fabsf(static_cast<float>(attackPosition.Y) - cameraPosition.Y) > viewRadiusY)
					{
						return;
					}

					Vector iso = WorldToIso(
						static_cast<float>(attackPosition.X) - cameraPosition.X,
						static_cast<float>(attackPosition.Y) - cameraPosition.Y,
						originX,
						originY
					);

					if (weaponType == WeaponType::Sword || weaponType == WeaponType::Axe)
					{
						drawSlashTile(iso, color, weaponType);
					}
					else if (weaponType == WeaponType::Magic)
					{
						const bool centerFire = MapManager::GetInstance()->IsTypeExist(attackPosition, MapObjectType::Monster);
						drawFireTile(iso, centerFire);
					}
					else
					{
						drawDefaultAttackTile(iso, color);
					}
				};

			Player* player = SceneManager::GetInstance()->GetPlayer();
			if (player != nullptr)
			{
				UCombatComponent* combatComponent = player->GetComponent<UCombatComponent>();
				if (combatComponent != nullptr && combatComponent->IsAttackVisible())
				{
					WeaponType weaponType = WeaponType::NONE;
					if (UEquipmentComponent* equipmentComponent = player->GetComponent<UEquipmentComponent>())
					{
						weaponType = equipmentComponent->GetCurrentWeaponType();
					}

					for (const Vector& attackPosition : combatComponent->GetAttackValue())
					{
						drawAttackTile(attackPosition, CC_RED, weaponType);
					}
				}
			}

			vector<AObject*>& objects = SceneManager::GetInstance()->GetObjects();
			for (AObject* object : objects)
			{
				Monster* monster = dynamic_cast<Monster*>(object);
				if (monster == nullptr || !monster->IsAttackVisible())
				{
					continue;
				}

				for (const Vector& attackPosition : monster->GetAttackValue())
				{
					drawAttackTile(attackPosition, CC_MAGENTA, WeaponType::NONE);
				}
			}
		};

	vector<Vector> shopPositions;
	vector<Vector> crystalPositions;

	const int startY = max(0, static_cast<int>(cameraPosition.Y) - viewRadiusY);
	const int endY = min(static_cast<int>(MAP_MAX_Y) - 1, static_cast<int>(cameraPosition.Y) + viewRadiusY);
	const int startX = max(0, static_cast<int>(cameraPosition.X) - viewRadiusX);
	const int endX = min(static_cast<int>(MAP_MAX_X) - 1, static_cast<int>(cameraPosition.X) + viewRadiusX);

	for (int y = startY; y <= endY; ++y)
	{
		for (int x = startX; x <= endX; ++x)
		{
			MapObjectType mapType = MapManager::GetInstance()->GetType(y, x);
			drawIsoTile(x, y, mapType == MapObjectType::Wall);
			if (mapType == MapObjectType::Shop)
			{
				shopPositions.push_back(WorldToIso(
					static_cast<float>(x) - cameraPosition.X,
					static_cast<float>(y) - cameraPosition.Y,
					originX,
					originY
				));
			}
			else if (mapType == MapObjectType::Crystal)
			{
				crystalPositions.push_back(WorldToIso(
					static_cast<float>(x) - cameraPosition.X,
					static_cast<float>(y) - cameraPosition.Y,
					originX,
					originY
				));
			}
		}
	}

	for (const Vector& shopPosition : shopPositions)
	{
		drawIsoShop(shopPosition);
	}

	for (const Vector& crystalPosition : crystalPositions)
	{
		drawIsoCrystal(crystalPosition);
	}

	drawAttackPositions();

	vector<AObject*>& objects = SceneManager::GetInstance()->GetObjects();
	vector<AObject*> sortedObjects;

	for (AObject* object : objects)
	{
		if (object == nullptr || object->IsDestroy())
		{
			continue;
		}

		sortedObjects.push_back(object);
	}

	sort(sortedObjects.begin(), sortedObjects.end(), [](AObject* left, AObject* right)
		{
			Vector leftPosition = left->GetPosition();
			Vector rightPosition = right->GetPosition();

			return (leftPosition.X + leftPosition.Y) < (rightPosition.X + rightPosition.Y);
		});

	for (AObject* object : sortedObjects)
	{
		FVector renderPosition;
		wchar_t objectIcon = L'?';
		WORD objectAttribute = MakeAttribute(CC_WHITE);
		Monster* monsterForHpBar = nullptr;
		Player* playerForBars = nullptr;
		Projectile* projectileForRender = nullptr;

		if (Player* player = dynamic_cast<Player*>(object))
		{
			playerForBars = player;
			float moveAlpha = 1.0f;
			UMoveComponent* moveComponent = player->GetComponent<UMoveComponent>();
			if (moveComponent != nullptr)
			{
				moveAlpha = moveComponent->GetMoveAlpha();

				switch (moveComponent->GetFacingDirection())
				{
				case EDirection::UP:
					objectIcon = L'↗';
					break;
				case EDirection::DOWN:
					objectIcon = L'↙';
					break;
				case EDirection::LEFT:
					objectIcon = L'↖';
					break;
				case EDirection::RIGHT:
					objectIcon = L'↘';
					break;
				case EDirection::NONE:
				default:
					objectIcon = L'@';
					break;
				}
			}
			else
			{
				objectIcon = L'@';
			}

			renderPosition = InterpolatePosition(player->GetPrevPosition(), GetRenderTargetPosition(player), moveAlpha);
			objectAttribute = MakeAttribute(player->IsHitFlashActive() ? CC_WHITE : CC_YELLOW);
		}
		else if (Monster* monster = dynamic_cast<Monster*>(object))
		{
			renderPosition = InterpolatePosition(monster->GetPrevPosition(), GetRenderTargetPosition(monster), monster->GetMoveAlpha());
			objectIcon = GetMonsterIcon(monster);
			objectAttribute = MakeAttribute(monster->IsHitFlashActive() ? CC_WHITE : GetMonsterColor(monster));
			monsterForHpBar = monster;
		}
		else if (Projectile* projectile = dynamic_cast<Projectile*>(object))
		{
			renderPosition = InterpolatePosition(projectile->GetPrevPosition(), projectile->GetPosition(), 1.0f);
			objectIcon = GetDirectionArrow(projectile->GetDirection());
			objectAttribute = MakeAttribute(CC_CYAN);
			projectileForRender = projectile;
		}
		else
		{
			renderPosition = InterpolatePosition(object->GetPrevPosition(), object->GetPosition(), 1.0f);
		}

		if (fabsf(renderPosition.X - 0.5f - cameraPosition.X) > viewRadiusX ||
			fabsf(renderPosition.Y - 0.5f - cameraPosition.Y) > viewRadiusY)
		{
			continue;
		}

		Vector iso = WorldToIso(
			renderPosition.X - 0.5f - cameraPosition.X,
			renderPosition.Y - 0.5f - cameraPosition.Y,
			originX,
			originY
		);
		if (monsterForHpBar != nullptr)
		{
			drawIsoMonsterActor(iso, monsterForHpBar, objectAttribute);
		}
		else if (projectileForRender != nullptr)
		{
			renderManager->PutCell(iso.Y - 3, iso.X, objectIcon, objectAttribute);
		}
		else
		{
			if (Player* player = dynamic_cast<Player*>(object))
			{
				drawTeleportEffects(player->GetComponent<UCombatComponent>());
				drawIsoPlayerActor(iso, objectIcon, objectAttribute, player);
			}
			else
			{
				drawIsoActor(iso, objectIcon, objectAttribute, false);
			}
		}
		drawPlayerBars(iso, playerForBars);
		drawLevelUpEffect(iso, playerForBars);
		drawItemUseEffect(iso, playerForBars);
		drawMonsterNameLabel(iso, monsterForHpBar);
		drawMonsterHpBar(iso, monsterForHpBar);
		drawObjectHitEffect(iso, object);
		if (dynamic_cast<Player*>(object) == nullptr && object->ShouldShowDamageText())
		{
			renderManager->AddRender(iso.Y - 9, iso.X - 1, L"-" + to_wstring(object->GetLastDamage()));
		}
	}

	renderManager->AddRender(1, 1, L"ISO");
}

void ViewportManager::OpenInventory()
{
	/*if (!inventoryComponent) return;
	if (InputManager::GetInstance()->IsKeyTap(KeyCode::I))
	{
		if (!inventoryComponent->GetOpenedInventory())	inventoryComponent->OpenInventory();
		else											inventoryComponent->CloseInventory();
	}

	bIsInvenOpen = inventoryComponent->GetOpenedInventory();*/
}

void ViewportManager::OpenExitDialog()
{
	if (Dialog.IsOpen())
	{
		return;
	}

	if (InputManager::GetInstance()->IsKeyTap(KeyCode::ESCAPE))
	{
		Dialog.OpenExitConfirm();
	}
}

void ViewportManager::OpenBattleUI()
{
	if (!Battle.IsOpen() && InputManager::GetInstance()->IsKeyTap(KeyCode::M))
	{
		Battle.Open();
	}
}

void ViewportManager::ShowMessageDialog(const wstring& Message, float Duration)
{
	Dialog.OpenMessage(Message, Duration);
}

void ViewportManager::AddItemLog(const UItem* Item)
{
	ItemLog.AddItemLog(Item);
}

void ViewportManager::StartFadeOut(float Duration)
{
	FadeState = ViewportFadeState::FadeOut;
	FadeDuration = max(0.01f, Duration);
	FadeElapsed = 0.0f;
}

void ViewportManager::StartFadeIn(float Duration)
{
	FadeState = ViewportFadeState::FadeIn;
	FadeDuration = max(0.01f, Duration);
	FadeElapsed = 0.0f;
}

void ViewportManager::TickFade(float DeltaTime)
{
	if (FadeState == ViewportFadeState::None)
	{
		return;
	}

	FadeElapsed += DeltaTime;
	if (FadeElapsed >= FadeDuration)
	{
		FadeElapsed = FadeDuration;
		FadeState = ViewportFadeState::None;
	}
}

void ViewportManager::RenderFade()
{
	if (FadeState == ViewportFadeState::None)
	{
		return;
	}

	DisplayManager* renderer = DisplayManager::GetInstance();
	float alpha = Clamp01(FadeElapsed / FadeDuration);
	if (FadeState == ViewportFadeState::FadeIn)
	{
		alpha = 1.0f - alpha;
	}

	int step = 6;
	wchar_t fadeCharacter = L'\x2591';
	if (alpha >= 0.8f)
	{
		step = 1;
		fadeCharacter = L' ';
	}
	else if (alpha >= 0.6f)
	{
		step = 2;
		fadeCharacter = L'\x2593';
	}
	else if (alpha >= 0.4f)
	{
		step = 3;
		fadeCharacter = L'\x2592';
	}
	else if (alpha >= 0.2f)
	{
		step = 4;
		fadeCharacter = L'\x2591';
	}

	const WORD attribute = renderer->MakeConsoleAttribute(CC_BLACK, CC_BLACK);
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = (y + step) % step; x < SCREEN_WIDTH; x += step)
		{
			renderer->PutCell(y, x, fadeCharacter, attribute);
		}
	}
}

Vector ViewportManager::WorldToIso(float WorldX, float WorldY, int OriginX, int OriginY)
{
	constexpr int TileHalfWidth = 8;
	constexpr int TileHalfHeight = 2;

	return {
		OriginX + static_cast<int>(roundf((WorldX - WorldY) * TileHalfWidth)),
		OriginY + static_cast<int>(roundf((WorldX + WorldY) * TileHalfHeight))
	};
}

Vector ViewportManager::GetISOPosition()
{
	Player* currentPlayer = SceneManager::GetInstance()->GetPlayer();
	if (currentPlayer == nullptr)
	{
		return { 0, 0 };
	}

	float moveAlpha = 1.0f;
	if (UMoveComponent* playerMoveComponent = currentPlayer->GetComponent<UMoveComponent>())
	{
		moveAlpha = playerMoveComponent->GetMoveAlpha();
	}

	FVector playerPosition = InterpolatePosition(currentPlayer->GetPrevPosition(), GetRenderTargetPosition(currentPlayer), moveAlpha);
	FVector cameraPosition = playerPosition;
	cameraPosition.X -= 0.5f;
	cameraPosition.Y -= 0.5f;

	return WorldToIso(
		playerPosition.X - 0.5f - cameraPosition.X,
		playerPosition.Y - 0.5f - cameraPosition.Y,
		SCREEN_WIDTH / 2,
		SCREEN_HEIGHT / 3
	);
}

void ViewportManager::Tick(float DeltaTime)
{
	if (!PlayerPtr)
	{
		PlayerPtr = SceneManager::GetInstance()->GetPlayer();
	}
	if (!inventoryComponent)
		inventoryComponent = PlayerPtr != nullptr ? PlayerPtr->GetComponent<UInventoryComponent>() : nullptr;

	ItemLog.Tick(DeltaTime);

	if (Dialog.IsOpen())
	{
		Dialog.Tick(DeltaTime);
		return;
	}

	if (Battle.IsOpen())
	{
		Battle.Tick(DeltaTime);
		return;
	}

	if (inventoryComponent != nullptr && inventoryComponent->GetOpenedInventory() && InputManager::GetInstance()->IsKeyTap(KeyCode::ESCAPE))
	{
		inventoryComponent->CloseInventory();
		bIsInvenOpen = false;
		return;
	}
	
	OpenInventory();
	if (inventoryComponent != nullptr && inventoryComponent->GetOpenedInventory())
	{
		bIsInvenOpen = true;
		return;
	}

	OpenBattleUI();
	OpenExitDialog();
}

void ViewportManager::BeginPlay()
{
}

void ViewportManager::Render()
{
	RenderObject();
	RenderUI();
}

void ViewportManager::RenderObject()
{	
	// Render2Dto3D();
	Render2DtoISO();
	// if (bIso) Render2DtoISO();
	// else      Render2Dto3D();
}

void ViewportManager::RenderUI()
{
	PlayerStatus.Render();
	if (inventoryComponent != nullptr)
	{
		bIsInvenOpen = inventoryComponent->GetOpenedInventory();
	}
	if (bIsInvenOpen)
	{
		if (inventoryComponent != nullptr && inventoryComponent->GetOnShop())
		{
			Shop.Render();
		}
		else
		{
			Inventory.Render();
		}
	}

	ItemLog.Render();
	Battle.Render();
	Dialog.Render();
}

void ViewportManager::ResetRuntimeCache()
{
	PlayerPtr = nullptr;
	moveComponent = nullptr;
	combatComponent = nullptr;
	inventoryComponent = nullptr;
	bIsInvenOpen = false;

	PlayerStatus.ResetCache();
	Inventory.ResetCache();
	Shop.ResetCache();
	ItemLog.Reset();
}

void ViewportManager::Render2Dto3D()
{
	DisplayManager* renderManager = DisplayManager::GetInstance();
	if (!PlayerPtr)
	{
		PlayerPtr = SceneManager::GetInstance()->GetPlayer();
	}

	float playerX = 1.0f;
	float playerY = 1.0f;
	float playerA = PI;

	if (PlayerPtr != nullptr)
	{
		float moveAlpha = 1.0f;
		if (!moveComponent)
			moveComponent = PlayerPtr->GetComponent<UMoveComponent>();

		if (moveComponent != nullptr)
		{
			moveAlpha = moveComponent->GetMoveAlpha();
		}

		FVector playerPosition = InterpolatePosition(PlayerPtr->GetPrevPosition(), PlayerPtr->GetPosition(), moveAlpha);
		playerX = playerPosition.X;
		playerY = playerPosition.Y;

		if (moveComponent != nullptr)
		{
			const float previousAngle = DirectionToAngle(moveComponent->GetPreviousFacingDirection());
			const float currentAngle = DirectionToAngle(moveComponent->GetFacingDirection());
			playerA = InterpolateAngle(previousAngle, currentAngle, moveComponent->GetTurnAlpha());
		}
	}
	
	float fov = PI / 8.0f;
	float depth = 36.0f;

	vector<float> wallDepths(SCREEN_WIDTH, depth);
	vector<WallFace> wallFaces(SCREEN_WIDTH, WallFace::None);

	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		float screenRatio = (static_cast<float>(x) + 0.5f) / static_cast<float>(SCREEN_WIDTH);
		float rayAngle = playerA + (screenRatio - 0.5f) * fov;

		float distanceToWall = 0.0f;
		bool hitWall = false;
		WallFace hitFace = WallFace::None;

		float eyeX = sinf(rayAngle);
		float eyeY = cosf(rayAngle);
		int previousTestX = static_cast<int>(playerX);
		int previousTestY = static_cast<int>(playerY);

		while (!hitWall && distanceToWall < depth)
		{
			distanceToWall += 0.1f;

			int testX = static_cast<int>(playerX + eyeX * distanceToWall);
			int testY = static_cast<int>(playerY + eyeY * distanceToWall);

			if (testX < 0 || testX >= MAP_MAX_X || testY < 0 || testY >= MAP_MAX_Y)
			{
				hitWall = true;
				distanceToWall = depth;
			}
			else
			{
				if (MapManager::GetInstance()->IsTypeExist(testY, testX, MapObjectType::Wall))
				{
					if (testX > previousTestX)      hitFace = WallFace::Left;
					else if (testX < previousTestX) hitFace = WallFace::Right;
					else if (testY > previousTestY) hitFace = WallFace::Up;
					else if (testY < previousTestY) hitFace = WallFace::Down;

					hitWall = true;
				}
				else
				{
					previousTestX = testX;
					previousTestY = testY;
				}
			}
		}

		wallDepths[x] = distanceToWall;
		wallFaces[x] = hitFace;

		int ceiling = static_cast<int>(SCREEN_HEIGHT / 2.0f - SCREEN_HEIGHT / distanceToWall);
		int floor = SCREEN_HEIGHT - ceiling;

		WORD wallAttribute = GetWallAttribute(hitFace, distanceToWall, depth);

		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			if (y < ceiling)
			{
				renderManager->PutCell(y, x, L' ', MakeAttribute(CC_DARKGRAY));
			}
			else if (y >= ceiling && y <= floor)
			{
				bool edge = y == ceiling || y == floor || x == 0 || x == SCREEN_WIDTH - 1;
				if (x > 0 && wallFaces[x - 1] != hitFace)
				{
					edge = true;
				}
				renderManager->PutCell(y, x, GetWallCharacter(distanceToWall, depth, edge), wallAttribute);
			}
			else
			{
				float b = 1.0f - (((float)y - SCREEN_HEIGHT / 2.0f) / ((float)SCREEN_HEIGHT / 2.0f));
				if (b < 0.25f)      renderManager->PutCell(y, x, L'#', MakeAttribute(CC_DARKGRAY));
				else if (b < 0.5f)  renderManager->PutCell(y, x, L'x', MakeAttribute(CC_DARKGRAY));
				else if (b < 0.75f) renderManager->PutCell(y, x, L'.', MakeAttribute(CC_GRAY));
				else                renderManager->PutCell(y, x, L' ', MakeAttribute(CC_GRAY));
			}
		}
	}

	vector<AObject*>& objects = SceneManager::GetInstance()->GetObjects();
	for (AObject* object : objects)
	{
		if (object == nullptr || object == PlayerPtr || object->IsDestroy())
		{
			continue;
		}

		Monster* monster = dynamic_cast<Monster*>(object);
		Projectile* projectile = dynamic_cast<Projectile*>(object);
		if (monster == nullptr && projectile == nullptr)
		{
			continue;
		}

		FVector enemyPosition = monster != nullptr
			? InterpolatePosition(monster->GetPrevPosition(), GetRenderTargetPosition(monster), monster->GetMoveAlpha())
			: InterpolatePosition(projectile->GetPrevPosition(), projectile->GetPosition(), 1.0f);
		float enemyX = enemyPosition.X;
		float enemyY = enemyPosition.Y;
		float vecX = enemyX - playerX;
		float vecY = enemyY - playerY;
		float distanceFromPlayer = sqrtf(vecX * vecX + vecY * vecY);

		if (distanceFromPlayer <= 0.01f || distanceFromPlayer >= depth)
		{
			continue;
		}

		float enemyAngle = atan2f(vecX, vecY) - playerA;
		while (enemyAngle < -PI) enemyAngle += 2.0f * PI;
		while (enemyAngle > PI) enemyAngle -= 2.0f * PI;

		if (fabsf(enemyAngle) > fov / 2.0f)
		{
			continue;
		}

		int enemyScreenX = static_cast<int>((0.5f + enemyAngle / fov) * SCREEN_WIDTH);
		int enemyHeight = monster != nullptr
			? max(1, static_cast<int>(SCREEN_HEIGHT / distanceFromPlayer))
			: 1;
		int enemyWidth = max(1, enemyHeight / 2);
		int enemyTop = max(0, SCREEN_HEIGHT / 2 - enemyHeight / 2);
		int enemyBottom = min(SCREEN_HEIGHT - 1, SCREEN_HEIGHT / 2 + enemyHeight / 2);
		int enemyLeft = max(0, enemyScreenX - enemyWidth / 2);
		int enemyRight = min(SCREEN_WIDTH - 1, enemyScreenX + enemyWidth / 2);

		for (int x = enemyLeft; x <= enemyRight; ++x)
		{
			if (distanceFromPlayer >= wallDepths[x])
			{
				continue;
			}

			wallDepths[x] = distanceFromPlayer;

			wchar_t objectCharacter = monster != nullptr ? GetMonsterIcon(monster) : GetDirectionArrow(projectile->GetDirection());
			WORD objectAttribute = monster != nullptr
				? MakeAttribute(monster->IsHitFlashActive() ? CC_WHITE : GetMonsterColor(monster))
				: MakeAttribute(CC_CYAN);

			for (int y = enemyTop; y <= enemyBottom; ++y)
			{
				renderManager->PutCell(y, x, objectCharacter, objectAttribute);
			}
		}

		if (monster != nullptr && monster->ShouldShowDamageText())
		{
			renderManager->AddRender(max(0, enemyTop - 1), max(0, enemyScreenX - 2), L"-" + to_wstring(monster->GetLastDamage()));
		}
	}

	if (!combatComponent)
		combatComponent = PlayerPtr->GetComponent<UCombatComponent>();
	
	if (combatComponent != nullptr && combatComponent->IsAttackVisible())
	{
		const int centerX = SCREEN_WIDTH / 2;
		const int centerY = SCREEN_HEIGHT / 2;
		renderManager->PutCell(centerY - 1, centerX, L'|', MakeAttribute(CC_RED));
		renderManager->PutCell(centerY, centerX - 1, L'-', MakeAttribute(CC_RED));
		renderManager->PutCell(centerY, centerX, L'X', MakeAttribute(CC_RED));
		renderManager->PutCell(centerY, centerX + 1, L'-', MakeAttribute(CC_RED));
		renderManager->PutCell(centerY + 1, centerX, L'|', MakeAttribute(CC_RED));
	}
}
