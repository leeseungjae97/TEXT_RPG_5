#include "RenderManager.h"

#include "../Define.h"
#include "../Item/Item.h"

namespace
{
    WORD MakeConsoleAttribute(int Color, int BackgroundColor = CC_BLACK)
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

void RenderManager::Render()
{    
    DrawScreen();
    ClearScreen();
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
        WORD attribute = MakeConsoleAttribute(CC_GRAY);

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
			PutCell(drawY, drawX, tile, MakeConsoleAttribute(CC_GRAY));
		}
	}
}

void RenderManager::DrawLine(int StartY, int StartX, int EndY, int EndX, wchar_t Character, int Color, int BgColor)
{
	int deltaX = abs(EndX - StartX);
	int deltaY = abs(EndY - StartY);
	int stepX = StartX < EndX ? 1 : -1;
	int stepY = StartY < EndY ? 1 : -1;
	int error = deltaX - deltaY;
	WORD attribute = MakeConsoleAttribute(Color, BgColor);

	while (true)
	{
		PutCell(StartY, StartX, Character, attribute);

		if (StartX == EndX && StartY == EndY)
		{
			break;
		}

		int doubledError = error * 2;
		if (doubledError > -deltaY)
		{
			error -= deltaY;
			StartX += stepX;
		}

		if (doubledError < deltaX)
		{
			error += deltaX;
			StartY += stepY;
		}
	}
}

void RenderManager::DrawBox(int Y, int X, int Width, int Height)
{
	if (Width < 2 || Height < 2)
	{
		return;
	}

	DrawLine(Y, X, Y, X + Width - 1, L'\x2500');
	DrawLine(Y + Height - 1, X, Y + Height - 1, X + Width - 1, L'\x2500');
	DrawLine(Y, X, Y + Height - 1, X, L'\x2502');
	DrawLine(Y, X + Width - 1, Y + Height - 1, X + Width - 1, L'\x2502');

	AddRender(Y, X, L"\x250c");
	AddRender(Y, X + Width - 1, L"\x2510");
	AddRender(Y + Height - 1, X, L"\x2514");
	AddRender(Y + Height - 1, X + Width - 1, L"\x2518");
}

void RenderManager::DrawItemSlot(int Y, int X, int Width, int Height, const UItem* item)
{
    // DrawBox(Y, X, Width, Height);
    //
    //
    // if (item == nullptr)
    // {
    //     return;
    // }

    // WORD iconAttribute = MakeConsoleAttribute(CC_CYAN);
    // if (item->Type == ItemType::Equipment)
    // {
    //     iconAttribute = MakeConsoleAttribute(CC_WHITE);
    // }
    // else if (itemInfo.Type == ItemType::Usable)
    // {
    //     iconAttribute = MakeConsoleAttribute(CC_GREEN);
    // }
    // else if (itemInfo.Type == ItemType::Misc)
    // {
    //     iconAttribute = MakeConsoleAttribute(CC_YELLOW);
    // }
    //
    // wchar_t icon = GetItemIcon(item);
    // PutCell(Y + 2, X + Width / 2, icon, iconAttribute);
    //
    // wstring itemName = ToWideString(itemInfo.Name);
    // int maxNameWidth = max(1, Width - 2);
    // if (GetTextDisplayWidth(itemName) > maxNameWidth)
    // {
    //     itemName = TrimTextToDisplayWidth(itemName, maxNameWidth);
    // }
    //
    // int nameX = X + 1 + max(0, (maxNameWidth - GetTextDisplayWidth(itemName)) / 2);
    // AddRender(Y + Height - 2, nameX, itemName);
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

	switch (item->GetItemInfo().Type)
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
            attributes[i] = MakeConsoleAttribute(CC_GRAY);
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
        buffer[i].Attributes = attributes != nullptr ? attributes[i] : MakeConsoleAttribute(CC_GRAY);
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

