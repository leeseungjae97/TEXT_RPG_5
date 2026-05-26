#include "ViewportManager.h"

#include "InputManager.h"
#include "MapManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "../Component/MoveComponent.h"
#include "../Define.h"
#include "../Monster.h"
#include "../Player.h"

namespace
{
	constexpr float PI = 3.14159f;

	enum class WallFace
	{
		None,
		Left,
		Right,
		Up,
		Down
	};

	struct RenderPosition
	{
		float X;
		float Y;
	};

	struct IsoScreenPosition
	{
		int X;
		int Y;
	};

	WORD MakeAttribute(int Color, int BackgroundColor = CC_BLACK)
	{
		return static_cast<WORD>(((BackgroundColor & 0xf) << 4) | (Color & 0xf));
	}

	RenderPosition InterpolatePosition(const Vector& PrevPosition, const Vector& CurrentPosition, float Alpha)
	{
		Alpha = min(max(Alpha, 0.0f), 1.0f);

		return {
			static_cast<float>(PrevPosition.X) + static_cast<float>(CurrentPosition.X - PrevPosition.X) * Alpha + 0.5f,
			static_cast<float>(PrevPosition.Y) + static_cast<float>(CurrentPosition.Y - PrevPosition.Y) * Alpha + 0.5f
		};
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

	IsoScreenPosition WorldToIso(float WorldX, float WorldY, int OriginX, int OriginY)
	{
		constexpr int TileHalfWidth = 8;
		constexpr int TileHalfHeight = 2;

		return {
			OriginX + static_cast<int>(roundf((WorldX - WorldY) * TileHalfWidth)),
			OriginY + static_cast<int>(roundf((WorldX + WorldY) * TileHalfHeight))
		};
	}
}

void ViewportManager::Render2DtoISO()
{
	RenderManager* renderManager = RenderManager::GetInstance();
	vector<vector<Coordinate>>& Map = MapManager::GetInstance()->GetMap();

	if (Map.empty() || Map[0].empty())
	{
		return;
	}

	const int originX = SCREEN_WIDTH / 2;
	const int originY = SCREEN_HEIGHT / 3;
	constexpr int viewRadiusX = 8;
	constexpr int viewRadiusY = 8;

	Player* currentPlayer = SceneManager::GetInstance()->GetPlayer();
	RenderPosition cameraPosition = {
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

		cameraPosition = InterpolatePosition(currentPlayer->GetPrevPosition(), currentPlayer->GetPosition(), moveAlpha);
		cameraPosition.X -= 0.5f;
		cameraPosition.Y -= 0.5f;
	}

	auto drawIsoDiamond = [&](IsoScreenPosition iso, WORD attribute)
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

	auto drawIsoWall = [&](IsoScreenPosition iso)
		{
			constexpr int wallHeight = 6;
			constexpr int tileHalfWidth = 8;
			WORD topAttribute = MakeAttribute(CC_LIGHTGRAY);
			WORD sideAttribute = MakeAttribute(CC_GRAY);
			WORD darkSideAttribute = MakeAttribute(CC_DARKGRAY);

			IsoScreenPosition topIso = { iso.X, iso.Y - wallHeight };

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

	auto drawIsoActor = [&](IsoScreenPosition iso, wchar_t icon, WORD attribute, bool player)
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

	auto drawMonsterHpBar = [&](IsoScreenPosition iso, Monster* monster)
		{
			if (monster == nullptr)
			{
				return;
			}

			constexpr int barWidth = 8;
			int maxHealth = max(1, monster->GetMaxHealth());
			int currentHealth = min(max(monster->GetHealth(), 0), maxHealth);
			int filledWidth = static_cast<int>(roundf(static_cast<float>(currentHealth) / static_cast<float>(maxHealth) * barWidth));

			int startX = iso.X - barWidth / 2 - 1;
			int y = iso.Y - 6;

			renderManager->PutCell(y, startX, L'[', MakeAttribute(CC_GRAY));
			for (int i = 0; i < barWidth; ++i)
			{
				bool filled = i < filledWidth;
				renderManager->PutCell(y, startX + 1 + i, filled ? L'=' : L'-', MakeAttribute(filled ? CC_RED : CC_DARKGRAY));
			}
			renderManager->PutCell(y, startX + barWidth + 1, L']', MakeAttribute(CC_GRAY));
		};

	auto drawIsoTile = [&](int mapX, int mapY, bool wall)
		{
			IsoScreenPosition iso = WorldToIso(
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

	const int startY = max(0, static_cast<int>(cameraPosition.Y) - viewRadiusY);
	const int endY = min(static_cast<int>(Map.size()) - 1, static_cast<int>(cameraPosition.Y) + viewRadiusY);
	const int startX = max(0, static_cast<int>(cameraPosition.X) - viewRadiusX);
	const int endX = min(static_cast<int>(Map[0].size()) - 1, static_cast<int>(cameraPosition.X) + viewRadiusX);

	for (int y = startY; y <= endY; ++y)
	{
		for (int x = startX; x <= endX; ++x)
		{
			drawIsoTile(x, y, Map[y][x].Type == ObjectType::Wall);
		}
	}

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
		RenderPosition renderPosition;
		wchar_t objectIcon = L'?';
		WORD objectAttribute = MakeAttribute(CC_WHITE);
		Monster* monsterForHpBar = nullptr;

		if (Player* player = dynamic_cast<Player*>(object))
		{
			float moveAlpha = 1.0f;
			UMoveComponent* moveComponent = player->GetComponent<UMoveComponent>();
			if (moveComponent != nullptr)
			{
				moveAlpha = moveComponent->GetMoveAlpha();

				switch (moveComponent->GetFacingDirection())
				{
				case EDirection::UP:
					objectIcon = L'^';
					break;
				case EDirection::DOWN:
					objectIcon = L'v';
					break;
				case EDirection::LEFT:
					objectIcon = L'<';
					break;
				case EDirection::RIGHT:
					objectIcon = L'>';
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

			renderPosition = InterpolatePosition(player->GetPrevPosition(), player->GetPosition(), moveAlpha);
			objectAttribute = MakeAttribute(CC_YELLOW);
		}
		else if (Monster* monster = dynamic_cast<Monster*>(object))
		{
			renderPosition = InterpolatePosition(monster->GetPrevPosition(), monster->GetPosition(), monster->GetMoveAlpha());
			objectIcon = L'M';
			objectAttribute = MakeAttribute(CC_MAGENTA);
			monsterForHpBar = monster;
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

		IsoScreenPosition iso = WorldToIso(
			renderPosition.X - 0.5f - cameraPosition.X,
			renderPosition.Y - 0.5f - cameraPosition.Y,
			originX,
			originY
		);
		drawIsoActor(iso, objectIcon, objectAttribute, dynamic_cast<Player*>(object) != nullptr);
		drawMonsterHpBar(iso, monsterForHpBar);
	}

	renderManager->AddRender(1, 1, L"ISO");
}

void ViewportManager::Tick(float DeltaTime)
{
	bool bCurrentInvenKey = InputManager::GetInstance()->IsKeyPressed(KeyCode::_3);
	if (bCurrentInvenKey && !bPrevInvenKey)
	{
		bInven = !bInven;
	}

	bPrevInvenKey = bCurrentInvenKey;

	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_1)) bIso = true;
	if (InputManager::GetInstance()->IsKeyPressed(KeyCode::_2)) bIso = false;
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
	if (bIso) Render2DtoISO();
	else      Render2Dto3D();
}

void ViewportManager::RenderUI()
{
	PlayerStatus.Render();
	if (bInven)
	{
		Inventory.Render();
	}
}

void ViewportManager::Render2Dto3D()
{
	RenderManager* renderManager = RenderManager::GetInstance();
	vector<vector<Coordinate>>& Map = MapManager::GetInstance()->GetMap();
	Player* player = SceneManager::GetInstance()->GetPlayer();

	float playerX = 1.0f;
	float playerY = 1.0f;
	float playerA = PI;

	if (player != nullptr)
	{
		float moveAlpha = 1.0f;
		UMoveComponent* moveComponent = player->GetComponent<UMoveComponent>();

		if (moveComponent != nullptr)
		{
			moveAlpha = moveComponent->GetMoveAlpha();
		}

		RenderPosition playerPosition = InterpolatePosition(player->GetPrevPosition(), player->GetPosition(), moveAlpha);
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
				if (Map[testY][testX].Type == ObjectType::Wall)
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
		if (object == nullptr || object == player || object->IsDestroy())
		{
			continue;
		}

		Monster* monster = dynamic_cast<Monster*>(object);
		if (monster == nullptr)
		{
			continue;
		}

		RenderPosition enemyPosition = InterpolatePosition(monster->GetPrevPosition(), monster->GetPosition(), monster->GetMoveAlpha());
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
		int enemyHeight = max(1, static_cast<int>(SCREEN_HEIGHT / distanceFromPlayer));
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

			for (int y = enemyTop; y <= enemyBottom; ++y)
			{
				renderManager->PutCell(y, x, L'M', MakeAttribute(CC_MAGENTA));
			}
		}
	}
}
