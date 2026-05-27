#pragma once

#include "../pch.h"

class DisplayManager;

enum class DialogType
{
	None,
	ExitConfirm,
	Message
};

class DialogUI
{
public:
	void OpenExitConfirm();
	void OpenMessage(const wstring& Message, float Duration = 1.5f);
	void Close();
	void Tick(float DeltaTime);
	void Render();
	bool IsOpen() const { return bOpen; }

private:
	bool bOpen = false;
	DialogType Type = DialogType::None;
	wstring MessageText;
	float RemainTime = 0.0f;
	DisplayManager* Renderer = nullptr;
};
