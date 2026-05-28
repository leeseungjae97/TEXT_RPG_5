#include "NameInputUI.h"

#include <conio.h>

#include "../Define.h"
#include "../Manager/DisplayManager.h"

void NameInputUI::Open()
{
	PlayerName.clear();
	CursorTimer = 0.0f;
	bShowCursor = true;

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

NameInputAction NameInputUI::Tick(float DeltaTime)
{
	CursorTimer += DeltaTime;
	if (CursorTimer >= 0.45f)
	{
		CursorTimer = 0.0f;
		bShowCursor = !bShowCursor;
	}

	while (_kbhit())
	{
		const wchar_t input = static_cast<wchar_t>(_getwch());

		if (input == 0 || input == 0xE0)
		{
			_getwch();
			continue;
		}

		if (input == L'\r')
		{
			if (!PlayerName.empty())
			{
				return NameInputAction::Start;
			}
			continue;
		}

		if (input == 27)
		{
			return NameInputAction::Cancel;
		}

		if (input == L'\b')
		{
			if (!PlayerName.empty())
			{
				PlayerName.pop_back();
			}
			continue;
		}

		if (iswprint(input))
		{
			DisplayManager* renderer = DisplayManager::GetInstance();
			if (renderer->GetTextDisplayWidth(PlayerName + input) <= MaxNameWidth)
			{
				PlayerName.push_back(input);
			}
		}
	}

	return NameInputAction::None;
}

void NameInputUI::Render()
{
	if (Renderer == nullptr)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

	const int width = 82;
	const int height = 20;
	const int x = max(1, (SCREEN_WIDTH - width) / 2);
	const int y = max(1, (SCREEN_HEIGHT - height) / 2);

	const WORD background = Renderer->MakeConsoleAttribute(CC_BLACK, CC_BLACK);

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			Renderer->PutCell(y + row, x + col, L' ', background);
		}
	}
	
	Renderer->DrawBox(y, x, width, height);
	Renderer->AddRender(y + 3, x + 6, L"당신의 이름을 입력하세요", CC_DARKYELLOW);
	Renderer->AddRender(y + 5, x + 6, L"Enter 입력 / Backspace 삭제 / ESC 돌아가기", CC_DARKGRAY);

	const int inputWidth = 44;
	const int inputX = x + 18;
	const int inputY = y + 10;
	Renderer->DrawBox(inputY - 1, inputX - 2, inputWidth, 3);

	wstring displayName = PlayerName;
	if (bShowCursor)
	{
		displayName += L"_";
	}

	if (displayName.empty())
	{
		Renderer->AddRender(inputY, inputX, L"이름", CC_DARKGRAY);
	}
	else
	{
		Renderer->AddRender(inputY, inputX, displayName, CC_WHITE);
	}

	Renderer->AddRender(y + 15, x + 6, L"DIABL5에 기록될 이름입니다.", CC_GRAY);
}
