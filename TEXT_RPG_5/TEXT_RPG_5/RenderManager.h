#pragma once
#include "Singleton.h"
#include "pch.h"

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
	void Init();
	void Render();
	void Destroy();

	void AddRender(int Y, int X, string Content);
	void AddRender(int Y, int X, vector<vector<int>>& Map);

private:
	wchar_t* screen = nullptr;
	HANDLE hConsole = INVALID_HANDLE_VALUE;
};
