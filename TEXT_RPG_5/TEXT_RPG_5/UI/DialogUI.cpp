#include "DialogUI.h"

#include "../Define.h"
#include "../GameInstance.h"
#include "../Manager/InputManager.h"
#include "../Manager/DisplayManager.h"

void DialogUI::OpenExitConfirm()
{
	bOpen = true;
	Type = DialogType::ExitConfirm;
	MessageText.clear();
	RemainTime = 0.0f;
}

void DialogUI::OpenMessage(const wstring& Message, float Duration)
{
	bOpen = true;
	Type = DialogType::Message;
	MessageText = Message;
	RemainTime = Duration;
}

void DialogUI::Close()
{
	bOpen = false;
	Type = DialogType::None;
	MessageText.clear();
	RemainTime = 0.0f;
}

void DialogUI::Tick(float DeltaTime)
{
	if (!bOpen)
	{
		return;
	}

	InputManager* input = InputManager::GetInstance();
	if (Type == DialogType::Message)
	{
		RemainTime -= DeltaTime;
		if (RemainTime <= 0.0f || input->IsKeyTap(KeyCode::ESCAPE))
		{
			Close();
		}
		return;
	}

	if (input->IsKeyTap(KeyCode::Z))
	{
		GameInstance::GetInstance()->RequestQuit();
		return;
	}

	if (input->IsKeyTap(KeyCode::ESCAPE) || input->IsKeyTap(KeyCode::X))
	{
		Close();
	}
}

void DialogUI::Render()
{
	if (!bOpen)
	{
		return;
	}

	if (!Renderer)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

	const int width = Type == DialogType::Message ? 44 : 40;
	const int height = Type == DialogType::Message ? 7 : 9;
	const int x = max(1, (SCREEN_WIDTH - width) / 2);
	const int y = max(1, (SCREEN_HEIGHT - height) / 2);

	WORD background = Renderer->MakeConsoleAttribute(CC_BLACK, CC_BLACK);
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			Renderer->PutCell(y + row, x + col, L' ', background);
		}
	}

	Renderer->DrawBox(y, x, width, height);
	if (Type == DialogType::Message)
	{
		wstring message = Renderer->TrimTextToDisplayWidth(MessageText, width - 4);
		Renderer->AddRender(y + 2, x + max(2, (width - Renderer->GetTextDisplayWidth(message)) / 2), message);
		Renderer->AddRender(y + 4, x + 15, L"(ESC) 닫기");
		return;
	}

	Renderer->AddRender(y + 2, x + 10, L"정말 종료할까요?");
	Renderer->AddRender(y + 5, x + 7, L"(Z) 종료        (ESC/X) 취소");
}
