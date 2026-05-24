#include "RenderManager.h"

#include "InputManager.h"
#include "MapManager.h"
#include "SceneManager.h"
#include "../Inventory.h"
#include "../Define.h"
#include "../Item.h"
#include "../Player.h"
#include "../Monster.h"
#include "../Component/MoveComponent.h"

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

    WORD MakeAttribute(int Color, int BackgroundColor = CC_BLACK)
    {
        return static_cast<WORD>(((BackgroundColor & 0xf) << 4) | (Color & 0xf));
    }

    bool IsWideCharacter(wchar_t Character)
    {
        return
            (Character >= 0x1100 && Character <= 0x11FF) ||
            (Character >= 0x2E80 && Character <= 0xA4CF) ||
            (Character >= 0xAC00 && Character <= 0xD7A3) ||
            (Character >= 0xF900 && Character <= 0xFAFF) ||
            (Character >= 0xFE10 && Character <= 0xFE6F) ||
            (Character >= 0xFF00 && Character <= 0xFF60) ||
            (Character >= 0xFFE0 && Character <= 0xFFE6);
    }

    int GetCharacterDisplayWidth(wchar_t Character)
    {
        if (Character == L'\0')
        {
            return 0;
        }

        return IsWideCharacter(Character) ? 2 : 1;
    }

    int GetTextDisplayWidth(const wstring& Text)
    {
        int width = 0;
        for (wchar_t character : Text)
        {
            width += GetCharacterDisplayWidth(character);
        }

        return width;
    }

    wstring TrimTextToDisplayWidth(const wstring& Text, int MaxWidth)
    {
        wstring result;
        int width = 0;

        for (wchar_t character : Text)
        {
            int characterWidth = GetCharacterDisplayWidth(character);
            if (width + characterWidth > MaxWidth)
            {
                break;
            }

            result.push_back(character);
            width += characterWidth;
        }

        return result;
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

    struct IsoScreenPosition
    {
        int X;
        int Y;
    };

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


RenderManager::~RenderManager()
{
    delete[] screen;
    screen = nullptr;

    delete[] attributes;
    attributes = nullptr;

    if (hConsole != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hConsole);
        hConsole = INVALID_HANDLE_VALUE;
    }
}

void RenderManager::BeginPlay()
{
    screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    attributes = new WORD[SCREEN_WIDTH * SCREEN_HEIGHT];
    hConsole = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        CONSOLE_TEXTMODE_BUFFER,
        nullptr
    );
    SetConsoleActiveScreenBuffer(hConsole);
}

void RenderManager::Tick(float DeltaTime)
{
    bool bCurrentInvenKey = InputManager::GetInstance()->IsKeyDown(eKeyCode::_3);
    if (bCurrentInvenKey && !bPrevInvenKey)
    {
        bInven = !bInven;
    }
    bPrevInvenKey = bCurrentInvenKey;
    
    if (InputManager::GetInstance()->IsKeyDown(eKeyCode::_1))
        bIso = true;
    
    if (InputManager::GetInstance()->IsKeyDown(eKeyCode::_2))
        bIso = false;
    
    if (bIso)
        _2DTOISO(DeltaTime);
    else
        _2DTO3D(DeltaTime);
    
    if (bInven)
        INVEN(DeltaTime);
}

void RenderManager::Render(float DeltaTime)
{
    ClearScreen();
    
    DrawScreen();
}

void RenderManager::Destroy()
{

}

void RenderManager::AddRender(int Y, int X, string Content)
{
	AddRender(Y, X, ToWideString(Content));
}

void RenderManager::AddRender(int Y, int X, wstring Content)
{
    int drawX = X;

	for (int i = 0; i < Content.length(); ++i)
	{
		int drawY = Y;
        int characterWidth = GetCharacterDisplayWidth(Content[i]);
        WORD attribute = MakeAttribute(CC_GRAY);

        if (characterWidth == 2)
        {
            PutCell(drawY, drawX, Content[i], attribute | COMMON_LVB_LEADING_BYTE);
            PutCell(drawY, drawX + 1, L' ', attribute | COMMON_LVB_TRAILING_BYTE);
            drawX += characterWidth;
            continue;
        }

		PutCell(drawY, drawX, Content[i], attribute);

        drawX += characterWidth;
	}
}

void RenderManager::AddRender(int Y, int X, vector<vector<int>>& Map)
{
	for (int y = 0; y < Map.size(); ++y)
	{
		for (int x = 0; x < Map[0].size(); ++x)
		{
			int drawX = X + x;
			int drawY = Y + y;

			if (drawX < 0 || drawX >= SCREEN_WIDTH || drawY < 0 || drawY >= SCREEN_HEIGHT)
			{
				continue;
			}

			wchar_t tile = Map[y][x] + '0';
			PutCell(drawY, drawX, tile, MakeAttribute(CC_GRAY));
		}
	}
}

void RenderManager::DrawBox(int Y, int X, int Width, int Height)
{
	if (Width < 2 || Height < 2)
	{
		return;
	}

	for (int x = 0; x < Width; ++x)
	{
		AddRender(Y, X + x, L"\x2500");
		AddRender(Y + Height - 1, X + x, L"\x2500");
	}

	for (int y = 0; y < Height; ++y)
	{
		AddRender(Y + y, X, L"\x2502");
		AddRender(Y + y, X + Width - 1, L"\x2502");
	}

	AddRender(Y, X, L"\x250c");
	AddRender(Y, X + Width - 1, L"\x2510");
	AddRender(Y + Height - 1, X, L"\x2514");
	AddRender(Y + Height - 1, X + Width - 1, L"\x2518");
}

void RenderManager::DrawItemSlot(int Y, int X, int Width, int Height, const UItem* item)
{
    DrawBox(Y, X, Width, Height);

    if (item == nullptr)
    {
        return;
    }

    WORD iconAttribute = MakeAttribute(CC_CYAN);
    if (item->Type == ItemType::Equipment)
    {
        iconAttribute = MakeAttribute(CC_WHITE);
    }
    else if (item->Type == ItemType::Usable)
    {
        iconAttribute = MakeAttribute(CC_GREEN);
    }
    else if (item->Type == ItemType::Misc)
    {
        iconAttribute = MakeAttribute(CC_YELLOW);
    }

    wchar_t icon = GetItemIcon(item);
    PutCell(Y + 2, X + Width / 2, icon, iconAttribute);

    wstring itemName = ToWideString(item->Name);
    int maxNameWidth = max(1, Width - 2);
    if (GetTextDisplayWidth(itemName) > maxNameWidth)
    {
        itemName = TrimTextToDisplayWidth(itemName, maxNameWidth);
    }

    int nameX = X + 1 + max(0, (maxNameWidth - GetTextDisplayWidth(itemName)) / 2);
    AddRender(Y + Height - 2, nameX, itemName);
}

void RenderManager::DrawInventoryPanel(int Y, int X, const vector<UItem*>& Items, int Capacity, int Columns, int Rows)
{
	const int slotWidth = 16;
	const int slotHeight = 7;

	AddRender(Y, X, L"인벤토리 (" + to_wstring(Items.size()) + L"/" + to_wstring(Capacity) + L")");

	for (int row = 0; row < Rows; ++row)
	{
		for (int col = 0; col < Columns; ++col)
		{
			int index = row * Columns + col;
			const UItem* item = index < Items.size() ? Items[index] : nullptr;
			DrawItemSlot(Y + 2 + row * (slotHeight - 1), X + col * (slotWidth - 1), slotWidth, slotHeight, item);
		}
	}
}

void RenderManager::DrawEquipmentPanel(int Y, int X, const vector<UItem*>& Items, int Columns, int Rows)
{
	const int slotWidth = 16;
	const int slotHeight = 7;

	AddRender(Y, X, L"장비");

	for (int row = 0; row < Rows; ++row)
	{
		for (int col = 0; col < Columns; ++col)
		{
			int index = row * Columns + col;
			const UItem* item = index < Items.size() ? Items[index] : nullptr;
			DrawItemSlot(Y + 2 + row * (slotHeight - 1), X + col * (slotWidth - 1), slotWidth, slotHeight, item);
		}
	}
}

wstring RenderManager::ToWideString(const string& Text)
{
	if (Text.empty())
	{
		return L"";
	}

	int length = MultiByteToWideChar(CP_UTF8, 0, Text.c_str(), -1, nullptr, 0);
	if (length <= 0)
	{
		return wstring(Text.begin(), Text.end());
	}

	wstring result(length - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, Text.c_str(), -1, &result[0], length);

	return result;
}

wchar_t RenderManager::GetItemIcon(const UItem* item)
{
	if (item == nullptr)
	{
		return L' ';
	}

	switch (item->Type)
	{
	case ItemType::Equipment:
		return L'@';
	case ItemType::Usable:
		return L'^';
	case ItemType::Misc:
		return L'■';
	default:
		return L'?';
	}
    return L' ';
}

void RenderManager::PutCell(int Y, int X, wchar_t Character, WORD Attribute)
{
    if (X < 0 || X >= SCREEN_WIDTH || Y < 0 || Y >= SCREEN_HEIGHT)
    {
        return;
    }

    const int Index = Y * SCREEN_WIDTH + X;
    screen[Index] = Character;

    if (attributes != nullptr)
    {
        attributes[Index] = Attribute;
    }
}

void RenderManager::ClearScreen()
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
    {
        screen[i] = L' ';
        if (attributes != nullptr)
        {
            attributes[i] = MakeAttribute(CC_GRAY);
        }
    }
}

void RenderManager::DrawScreen()
{
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        return;
    }

    screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = L'\0';

    vector<CHAR_INFO> buffer(SCREEN_WIDTH * SCREEN_HEIGHT);
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
    {
        buffer[i].Char.UnicodeChar = screen[i];
        buffer[i].Attributes = attributes != nullptr ? attributes[i] : MakeAttribute(CC_GRAY);
    }

    COORD bufferSize = { static_cast<SHORT>(SCREEN_WIDTH), static_cast<SHORT>(SCREEN_HEIGHT) };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = {
        0,
        0,
        static_cast<SHORT>(SCREEN_WIDTH - 1),
        static_cast<SHORT>(SCREEN_HEIGHT - 1)
    };

    WriteConsoleOutputW(hConsole, buffer.data(), bufferSize, bufferCoord, &writeRegion);
}

void RenderManager::SetColor(int Color, int BackgroundColor)
{
    Color &= 0xf;
    BackgroundColor &= 0xf;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
        (BackgroundColor << 4) | Color);
}

void RenderManager::SetFontColor(int Color)
{
    CONSOLE_SCREEN_BUFFER_INFO buff;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buff);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
        (buff.wAttributes & 0xf0) | (Color & 0xf));
}

void RenderManager::SetBackgroundColor(int BackgroundColor)
{
    CONSOLE_SCREEN_BUFFER_INFO buff;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buff);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
        ((BackgroundColor & 0xf) << 4) | (buff.wAttributes & 0xf));
}

int RenderManager::GetFontColor()
{
    CONSOLE_SCREEN_BUFFER_INFO buff;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buff);
    return buff.wAttributes & 0xf;
}

int RenderManager::GetBackgroundColor()
{
    CONSOLE_SCREEN_BUFFER_INFO buff;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buff);
    return (buff.wAttributes & 0xf0) >> 4;
}

void RenderManager::_2DTOISO(float DeltaTime)
{
    vector<vector<int>>& Map = MapManager::GetInstance()->GetMap();

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
                PutCell(iso.Y - tileHalfHeight + i, iso.X - i * stepX, L'/', attribute);
                PutCell(iso.Y - tileHalfHeight + i, iso.X + i * stepX, L'\\', attribute);
                PutCell(iso.Y + i, iso.X - tileHalfWidth + i * stepX, L'\\', attribute);
                PutCell(iso.Y + i, iso.X + tileHalfWidth - i * stepX, L'/', attribute);
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
                PutCell(y, iso.X - tileHalfWidth, L'|', darkSideAttribute);
                PutCell(y, iso.X + tileHalfWidth, L'|', sideAttribute);
            }

            PutCell(topIso.Y + 1, iso.X - 4, L'#', topAttribute);
            PutCell(topIso.Y + 1, iso.X, L'#', topAttribute);
            PutCell(topIso.Y + 1, iso.X + 4, L'#', topAttribute);

            PutCell(iso.Y - 4, iso.X - 7, L':', darkSideAttribute);
            PutCell(iso.Y - 3, iso.X - 7, L':', darkSideAttribute);
            PutCell(iso.Y - 2, iso.X + 7, L':', sideAttribute);
            PutCell(iso.Y - 1, iso.X + 7, L':', sideAttribute);
        };

    auto drawIsoActor = [&](IsoScreenPosition iso, wchar_t icon, WORD attribute, bool player)
        {
            WORD shadowAttribute = MakeAttribute(CC_DARKGRAY);
            PutCell(iso.Y, iso.X - 2, L'(', shadowAttribute);
            PutCell(iso.Y, iso.X - 1, L'_', shadowAttribute);
            PutCell(iso.Y, iso.X, L'_', shadowAttribute);
            PutCell(iso.Y, iso.X + 1, L')', shadowAttribute);

            PutCell(iso.Y - 4, iso.X, icon, attribute);
            PutCell(iso.Y - 3, iso.X - 1, player ? L'/' : L'{', attribute);
            PutCell(iso.Y - 3, iso.X, player ? L'|' : L'M', attribute);
            PutCell(iso.Y - 3, iso.X + 1, player ? L'\\' : L'}', attribute);
            PutCell(iso.Y - 2, iso.X - 1, L'/', attribute);
            PutCell(iso.Y - 2, iso.X, L'|', attribute);
            PutCell(iso.Y - 2, iso.X + 1, L'\\', attribute);
            PutCell(iso.Y - 1, iso.X - 1, L'/', attribute);
            PutCell(iso.Y - 1, iso.X + 1, L'\\', attribute);
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

            PutCell(y, startX, L'[', MakeAttribute(CC_GRAY));
            for (int i = 0; i < barWidth; ++i)
            {
                bool filled = i < filledWidth;
                PutCell(y, startX + 1 + i, filled ? L'=' : L'-', MakeAttribute(filled ? CC_RED : CC_DARKGRAY));
            }
            PutCell(y, startX + barWidth + 1, L']', MakeAttribute(CC_GRAY));
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
            drawIsoTile(x, y, Map[y][x] == 0);
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

    AddRender(1, 1, L"ISO");
    screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = L'\0';
}

void RenderManager::INVEN(float DeltaTime)
{
    // vector<UItem*>& container = InventoryManager::GetInstance()->GetContainer();
    // vector<UItem*> inventoryItems;
    // vector<UItem*> equipmentItems;
    //
    // for (UItem* item : container)
    // {
    //     if (item == nullptr)
    //     {
    //         continue;
    //     }
    //
    //     if (item->Type == ItemType::Equipment)
    //     {
    //         equipmentItems.push_back(item);
    //     }
    //     else
    //     {
    //         inventoryItems.push_back(item);
    //     }
    // }
    //
    // constexpr int inventoryColumns = 7;
    // constexpr int inventoryRows = 3;
    // constexpr int inventoryCapacity = inventoryColumns * inventoryRows;
    // constexpr int equipmentColumns = 2;
    // constexpr int equipmentRows = 3;
    //
    // const int inventoryX = 3;
    // const int inventoryY = 3;
    // const int equipmentX = inventoryX + inventoryColumns * 15 + 8;
    // const int equipmentY = inventoryY;
    //
    // DrawInventoryPanel(inventoryY, inventoryX, inventoryItems, inventoryCapacity, inventoryColumns, inventoryRows);
    // DrawEquipmentPanel(equipmentY, equipmentX, equipmentItems, equipmentColumns, equipmentRows);
    // screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = L'\0';
}

void RenderManager::_2DTO3D(float DeltaTime)
{
    vector<vector<int>>& Map = MapManager::GetInstance()->GetMap();
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
    // 시야각 (FOV)
    float fov = PI / 8.0f;
    // 최대 렌더링 거리
    float depth = 36.0f;
    ElapsedTime += DeltaTime;
	
    vector<float> wallDepths(SCREEN_WIDTH, depth);
    vector<WallFace> wallFaces(SCREEN_WIDTH, WallFace::None);

    // 화면 가로 픽셀만큼 레이캐스팅 수행
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        // 각 픽셀에 해당하는 레이의 상대 각도 계산
        float screenRatio = (static_cast<float>(x) + 0.5f) / static_cast<float>(SCREEN_WIDTH);
        float rayAngle = playerA + (screenRatio - 0.5f) * fov;

        float distanceToWall = 0.0f;
        bool hitWall = false;
        WallFace hitFace = WallFace::None;

        float eyeX = sinf(rayAngle); // 레이의 단위 방향 벡터 X
        float eyeY = cosf(rayAngle); // 레이의 단위 방향 벡터 Y
        int previousTestX = static_cast<int>(playerX);
        int previousTestY = static_cast<int>(playerY);

        // 벽을 만날 때까지 레이를 전진시킴
        while (!hitWall && distanceToWall < depth)
        {
            distanceToWall += 0.1f;

            int testX = (int)(playerX + eyeX * distanceToWall);
            int testY = (int)(playerY + eyeY * distanceToWall);

            // 레이가 맵 경계를 벗어난 경우
            if (testX < 0 || testX >= MAP_MAX_X || testY < 0 || testY >= MAP_MAX_Y)
            {
                hitWall = true;
                distanceToWall = depth; // 최대 거리 처리
            }
            else
            {
                // 벽과 충돌한 경우
                if (Map[testY][testX] == 0)
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

        // 거리에 따른 벽 높이(천장과 바닥의 위치) 계산
        int ceiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / ((float)distanceToWall);
        int floor = SCREEN_HEIGHT - ceiling;

        WORD wallAttribute = GetWallAttribute(hitFace, distanceToWall, depth);

        // 화면 세로줄 채우기
        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            if (y < ceiling)
            {
                PutCell(y, x, L' ', MakeAttribute(CC_DARKGRAY)); // 천장
            }
            else if (y >= ceiling && y <= floor)
            {
                bool edge = y == ceiling || y == floor || x == 0 || x == SCREEN_WIDTH - 1;
                if (x > 0 && wallFaces[x - 1] != hitFace)
                {
                    edge = true;
                }
                PutCell(y, x, GetWallCharacter(distanceToWall, depth, edge), wallAttribute); // 벽
            }
            else
            {
                // 바닥 (거리에 따른 음영 표현)
                float b = 1.0f - (((float)y - SCREEN_HEIGHT / 2.0f) / ((float)SCREEN_HEIGHT / 2.0f));
                if (b < 0.25)      PutCell(y, x, L'#', MakeAttribute(CC_DARKGRAY));
                else if (b < 0.5)  PutCell(y, x, L'x', MakeAttribute(CC_DARKGRAY));
                else if (b < 0.75) PutCell(y, x, L'.', MakeAttribute(CC_GRAY));
                else               PutCell(y, x, L' ', MakeAttribute(CC_GRAY));
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
                PutCell(y, x, L'M', MakeAttribute(CC_MAGENTA));
            }
        }
    }
    screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = '\0';
}
