#include "RenderManager.h"
#include "Define.h"

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

void RenderManager::Init()
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

}

void RenderManager::Render()
{
    ClearScreen();
    DrawScreen();
}

void RenderManager::Destroy()
{

}
void RenderManager::AddRender(int Y, int X, string Content)
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
void RenderManager::ClearScreen()
{
    //if (hConsole == INVALID_HANDLE_VALUE)
    //    return;

    //CONSOLE_SCREEN_BUFFER_INFO csbi = {};
    //if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    //    return;

    //DWORD cellCount = static_cast<DWORD>(csbi.dwSize.X) * static_cast<DWORD>(csbi.dwSize.Y);
    //COORD home = { 0, 0 };
    //DWORD written = 0;

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
    {
        screen[i] = L' ';
    }

    //FillConsoleOutputCharacter(hConsole, L' ', cellCount, home, &written);
    //FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, home, &written);
    //SetConsoleCursorPosition(hConsole, home);
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
