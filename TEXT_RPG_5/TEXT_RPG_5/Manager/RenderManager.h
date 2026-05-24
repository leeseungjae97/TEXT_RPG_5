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

struct Item;

class RenderManager : public Singleton<RenderManager>
{
public:
	RenderManager() {}
	~RenderManager();

public:
	void ClearScreen();
	void DrawScreen();

public:
	void Tick(float DeltaTime);
	void BeginPlay();
	void Render(float DeltaTime);
	void Destroy();

	void AddRender(int Y, int X, string Content);
	void AddRender(int Y, int X, vector<vector<int>>& Map);
	void AddRender(int Y, int X, wstring Content);

public:
	void DrawBox(int Y, int X, int Width, int Height);
	void DrawItemSlot(int Y, int X, int Width, int Height, const Item* item);
	void DrawInventoryPanel(int Y, int X, const vector<Item*>& Items, int Capacity, int Columns, int Rows);
	void DrawEquipmentPanel(int Y, int X, const vector<Item*>& Items, int Columns, int Rows);

public:
	void _2DTO3D(float DeltaTime);
	void _2DTOISO(float DeltaTime);

public:
	void SetColor(int Color, int Bgcolor);
	void SetFontColor(int Color);
	void SetBackgroundColor(int Bgcolor);
	int GetFontColor();
	int GetBackgroundColor();

private:
	float ElapsedTime = 0.0f;

private:
	wstring ToWideString(const string& Text);
	wchar_t GetItemIcon(const Item* item);
	void PutCell(int Y, int X, wchar_t Character, WORD Attribute);

	wchar_t* screen = nullptr;
	WORD* attributes = nullptr;
	HANDLE hConsole = INVALID_HANDLE_VALUE;
};
