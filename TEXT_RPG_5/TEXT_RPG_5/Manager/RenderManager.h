#pragma once
#include "../Singleton.h"
#include "../pch.h"

enum ConsoleColor {
	CC_BLACK,
	CC_DARKBLUE,
	CC_DARKGREEN,
	CC_DARKCYAN,
	CC_DARKRED,
	CC_DARKMAGENTA,
	CC_DARKYELLOW,
	CC_GRAY,
	CC_DARKGRAY,
	CC_BLUE,
	CC_GREEN,
	CC_CYAN,
	CC_RED,
	CC_MAGENTA,
	CC_YELLOW,
	CC_WHITE,
	CC_LIGHTGRAY = 7,
	CC_ORIGINAL = 7,
	CC_ORIGINALFONT = 7,
	CC_ORIGINALBG = 0
};

class UItem;

class RenderManager : public Singleton<RenderManager>
{
public:
	RenderManager() {}
	~RenderManager();

private:
	void ClearScreen();
	void DrawScreen();

public:
	void Tick(float DeltaTime) {}
	void BeginPlay();
	void Render();
	void Destroy();

	void AddRender(int Y, int X, string Content);
	void AddRender(int Y, int X, vector<vector<int>>& Map);
	void AddRender(int Y, int X, wstring Content);

public:
	void DrawLine(int StartY, int StartX, int EndY, int EndX, wchar_t Character = L'*', int Color = CC_GRAY, int BgColor = CC_BLACK);
	void DrawBox(int Y, int X, int Width, int Height);
	void PutCell(int Y, int X, wchar_t Character, WORD Attribute);

public:
	void SetColor(int Color, int Bgcolor);
	void SetFontColor(int Color);
	void SetBackgroundColor(int Bgcolor);
	int GetFontColor();
	int GetBackgroundColor();
	
	WORD MakeConsoleAttribute(int Color, int BackgroundColor = CC_BLACK);
	bool IsWideCharacter(wchar_t Character);

	int GetCharacterDisplayWidth(wchar_t Character);
	int GetTextDisplayWidth(const wstring& Text);
	wstring TrimTextToDisplayWidth(const wstring& Text, int MaxWidth);
	wstring ToWideString(const string& Text);

private:
	wchar_t* screen = nullptr;
	WORD* attributes = nullptr;
	HANDLE hConsole = INVALID_HANDLE_VALUE;
};
