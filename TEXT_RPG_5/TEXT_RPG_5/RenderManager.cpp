#include "RenderManager.h"
#include "MapManager.h"
#include "Define.h"
#include "Item.h"


RenderManager::~RenderManager()
{
    delete[] screen;
    screen = nullptr;

    if (hConsole != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hConsole);
        hConsole = INVALID_HANDLE_VALUE;
    }
}

void RenderManager::BeginPlay()
{
    screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
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
    //_2DTO3D(DeltaTime);
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
	for (int i = 0; i < Content.length(); ++i)
	{
		int drawX = X + i;
		int drawY = Y;

		if (drawX < 0 || drawX >= SCREEN_WIDTH || drawY < 0 || drawY >= SCREEN_HEIGHT)
		{
			continue;
		}
        
		screen[drawY * SCREEN_WIDTH + drawX] = Content[i];
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
			screen[drawY * SCREEN_WIDTH + drawX] = tile;
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

void RenderManager::DrawItemSlot(int Y, int X, int Width, int Height, const Item* item)
{
	//DrawBox(Y, X, Width, Height);

	//if (item == nullptr)
	//{
	//	return;
	//}

	//wchar_t icon[2] = { GetItemIcon(item), L'\0' };
	//AddRender(Y + 1, X + Width / 2, icon);

	//wstring itemName = ToWideString(item->Name);
	//int maxNameLength = Width - 2;
	//if (itemName.length() > maxNameLength)
	//{
	//	itemName = itemName.substr(0, maxNameLength);
	//}

	//int nameX = X + 1 + max(0, (maxNameLength - static_cast<int>(itemName.length())) / 2);
	//AddRender(Y + Height - 2, nameX, itemName);
}

void RenderManager::DrawInventoryPanel(int Y, int X, const vector<Item*>& Items, int Capacity, int Columns, int Rows)
{
	const int slotWidth = 14;
	const int slotHeight = 5;

	AddRender(Y, X, L"Inventory (" + to_wstring(Items.size()) + L"/" + to_wstring(Capacity) + L")");

	for (int row = 0; row < Rows; ++row)
	{
		for (int col = 0; col < Columns; ++col)
		{
			int index = row * Columns + col;
			const Item* item = index < Items.size() ? Items[index] : nullptr;
			DrawItemSlot(Y + 2 + row * (slotHeight - 1), X + col * (slotWidth - 1), slotWidth, slotHeight, item);
		}
	}
}

void RenderManager::DrawEquipmentPanel(int Y, int X, const vector<Item*>& Items, int Columns, int Rows)
{
	const int slotWidth = 14;
	const int slotHeight = 5;

	AddRender(Y, X, L"Equipment");

	for (int row = 0; row < Rows; ++row)
	{
		for (int col = 0; col < Columns; ++col)
		{
			int index = row * Columns + col;
			const Item* item = index < Items.size() ? Items[index] : nullptr;
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

	int length = MultiByteToWideChar(CP_ACP, 0, Text.c_str(), -1, nullptr, 0);
	if (length <= 0)
	{
		return wstring(Text.begin(), Text.end());
	}

	wstring result(length - 1, L'\0');
	MultiByteToWideChar(CP_ACP, 0, Text.c_str(), -1, &result[0], length);

	return result;
}

wchar_t RenderManager::GetItemIcon(const Item* item)
{
	//if (item == nullptr)
	//{
	//	return L' ';
	//}

	//switch (item->Type)
	//{
	//case ItemType::Equipment:
	//	return L'E';
	//case ItemType::Usable:
	//	return L'U';
	//case ItemType::Misc:
	//	return L'*';
	//default:
	//	return L'?';
	//}
    return L' ';
}

void RenderManager::ClearScreen()
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
    {
        screen[i] = L' ';
    }
}

void RenderManager::DrawScreen()
{
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        return;
    }

    DWORD dwBytesWritten = 0;
    screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = L'\0';
    WriteConsoleOutputCharacterW(
        hConsole,
        screen,
        SCREEN_WIDTH * SCREEN_HEIGHT,
        { 0, 0 },
        &dwBytesWritten
    );
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

void RenderManager::_2DTO3D(float DeltaTime)
{
    vector<vector<int>>& Map = MapManager::GetInstance()->GetMap();

    // 플레이어 초기 상태 (위치, 시선 방향 각도)
    float playerX = 1.0f;
    float playerY = 1.0f;
    float playerA = 0.0f; // 시선 각도 (라디안)

    // 시야각 (FOV)
    float fov = 3.14159f / 4.0f;
    // 최대 렌더링 거리
    float depth = 16.0f;
    ElapsedTime += DeltaTime;

    // 게임 루프
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {

    }
    // 입력 처리 (조작법: A/D 회전, W/S 전진/후진)
    if (GetAsyncKeyState((unsigned short)'A') & 0x8000) playerA -= (2.0f) * ElapsedTime;
    if (GetAsyncKeyState((unsigned short)'D') & 0x8000) playerA += (2.0f) * ElapsedTime;

    if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
    {
        playerX += sinf(playerA) * 5.0f * ElapsedTime;
        playerY += cosf(playerA) * 5.0f * ElapsedTime;
        // 벽 충돌 검사
        if (Map[playerY][playerX] == 0)
        {
            playerX -= sinf(playerA) * 5.0f * ElapsedTime;
            playerY -= cosf(playerA) * 5.0f * ElapsedTime;
        }
    }
    if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
    {
        playerX -= sinf(playerA) * 5.0f * ElapsedTime;
        playerY -= cosf(playerA) * 5.0f * ElapsedTime;
        // 벽 충돌 검사
        if (Map[playerY][playerX] == 0)
        {
            playerX += sinf(playerA) * 5.0f * ElapsedTime;
            playerY += cosf(playerA) * 5.0f * ElapsedTime;
        }
    }

    // 화면 가로 픽셀만큼 레이캐스팅 수행
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        // 각 픽셀에 해당하는 레이의 상대 각도 계산
        float rayAngle = (playerA - fov / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * fov;

        float distanceToWall = 0.0f;
        bool hitWall = false;

        float eyeX = sinf(rayAngle); // 레이의 단위 방향 벡터 X
        float eyeY = cosf(rayAngle); // 레이의 단위 방향 벡터 Y

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
                    hitWall = true;
            }
        }

        // 거리에 따른 벽 높이(천장과 바닥의 위치) 계산
        int ceiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / ((float)distanceToWall);
        int floor = SCREEN_HEIGHT - ceiling;

        // 벽면에 음영 효과를 주기 위한 문자 선택
        wchar_t wallChar = ' ';
        if (distanceToWall <= depth / 4.0f)      wallChar = 0x2588; // 아주 가까움 (꽉 찬 블록)
        else if (distanceToWall < depth / 3.0f)  wallChar = 0x2593; // 가까움
        else if (distanceToWall < depth / 2.0f)  wallChar = 0x2592; // 중간
        else if (distanceToWall < depth)         wallChar = 0x2512; // 먼 거리
        else                                     wallChar = ' ';    // 보이지 않음

        // 화면 세로줄 채우기
        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            if (y < ceiling)
            {
                screen[y * SCREEN_WIDTH + x] = ' '; // 천장
            }
            else if (y >= ceiling && y <= floor)
            {
                screen[y * SCREEN_WIDTH + x] = wallChar; // 벽
            }
            else
            {
                // 바닥 (거리에 따른 음영 표현)
                float b = 1.0f - (((float)y - SCREEN_HEIGHT / 2.0f) / ((float)SCREEN_HEIGHT / 2.0f));
                if (b < 0.25)      screen[y * SCREEN_WIDTH + x] = '#';
                else if (b < 0.5)  screen[y * SCREEN_WIDTH + x] = 'x';
                else if (b < 0.75) screen[y * SCREEN_WIDTH + x] = '.';
                else               screen[y * SCREEN_WIDTH + x] = ' ';
            }
        }
    }
    screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = '\0';
}