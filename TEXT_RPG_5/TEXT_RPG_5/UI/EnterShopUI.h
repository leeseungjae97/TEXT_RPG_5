#pragma once

class DisplayManager;

class EnterShopUI
{
public:
	void Close();
	void Tick(float DeltaTime);
	void Render();
	
	void Open();
	bool IsOpen() const { return bOpen; }
	
private:
	bool bOpen = false;
	DisplayManager* Renderer = nullptr;
};
