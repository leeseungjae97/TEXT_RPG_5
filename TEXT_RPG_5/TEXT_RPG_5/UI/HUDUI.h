#pragma once

class DisplayManager;
class Player;
class UItem;

class HUDUI
{
public:
	void Render();
	
private:
	void StatusRender();
	void MapRender();
	void QuickSlotRender();
	void DrawStatusBar(int Y, int X, int Width, float Ratio, int FilledColor);
	wchar_t GetMapIcon(int MapY, int MapX);
	int GetMapIconColor(int MapY, int MapX);
	wchar_t GetItemIcon(const UItem* Item);

private:
	Player* PlayerPtr = nullptr;
	DisplayManager* Renderer = nullptr;
	
	int ShopColorIndex = 0;
	float ColorChangeInterval = 0.8f;
	float ColorChangeDuration = 0.0f;
};
