#include "GameOverUI.h"

#include "../Define.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/InputManager.h"

GameOverAction GameOverUI::Tick(float DeltaTime)
{
	StarTimer += DeltaTime;
	if (StarTimer >= 0.4f)
	{
		StarTimer = 0.0f;
		StarOffset = (StarOffset + 1) % 3;
	}

	InputManager* input = InputManager::GetInstance();
	if (input->IsKeyTap(KeyCode::UP) || input->IsKeyTap(KeyCode::DOWN))
	{
		SelectedIndex = 1 - SelectedIndex;
	}

	if (input->IsKeyTap(KeyCode::_1))
	{
		return GameOverAction::Retry;
	}
	if (input->IsKeyTap(KeyCode::_2) || input->IsKeyTap(KeyCode::ESCAPE))
	{
		return GameOverAction::MainMenu;
	}
	if (input->IsKeyTap(KeyCode::Z))
	{
		return SelectedIndex == 0 ? GameOverAction::Retry : GameOverAction::MainMenu;
	}

	return GameOverAction::None;
}

void GameOverUI::Render()
{
	if (Renderer == nullptr)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

	const int borderX = 2;
	const int borderY = 1;
	const int borderWidth = SCREEN_WIDTH - 4;
	const int borderHeight = SCREEN_HEIGHT - 2;
	Renderer->DrawBox(borderY, borderX, borderWidth, borderHeight);

	RenderTitle(42, 20);
	RenderGraveyard(SCREEN_WIDTH / 2 + 8, 5);
	RenderMenu(56, 35);
}

void GameOverUI::RenderTitle(int X, int Y)
{
	static const vector<wstring> title =
	{
		L"  ####    #    #   # #####     ####  #   # ##### #####",
		L" #       # #   ## ## #        #   #  #   # #     #   #",
		L" #  ##  #####  # # # ####     #   #  #   # ####  #####",
		L" #   #  #   #  #   # #        #   #   # #  #     #  # ",
		L"  ####  #   #  #   # #####     ####    #   ##### #   #"
	};

	for (int i = 0; i < static_cast<int>(title.size()); ++i)
	{
		Renderer->AddRender(Y + i, X, title[i], CC_DARKYELLOW);
	}
}

void GameOverUI::RenderGraveyard(int X, int Y)
{
	static const vector<wstring> sky =
	{
		L"             .          *        .",
		L"       .          .              ",
		L"                         .       ",
		L"             *                   "
	};

	for (int i = 0; i < static_cast<int>(sky.size()); ++i)
	{
		Renderer->AddRender(Y + i, X + ((i + StarOffset) % 3), sky[i], CC_DARKYELLOW);
	}

	static const vector<wstring> graveyard =
	{
		L"                         _..._",
		L"          .-.          .'     '.        .-.",
		L"         (   ).       /  _   _  \\      (   )",
		L"        (___(__)      | (_) (_) |     (__ )__)",
		L"             ||       |    ^    |       ||",
		L"      /\\     ||       |  \\___/  |       ||    /\\",
		L"     /  \\ ___||___    |_________|    ___||___/  \\",
		L"    /____\\  []   |       ||||       |   []  /____\\",
		L"       || |______|   ____||||____   |______| ||",
		L"       ||      |    /    ||||    \\     |     ||",
		L"   ____||______|___/____/||||\\____\\____|_____||____",
		L"      _/\\_        _/\\_       _/\\_        _/\\_",
		L"     / RIP\\      / RIP\\     / RIP\\      / RIP\\",
		L"     |____|      |____|     |____|      |____|",
		L"         \\         |    /\\    |        /",
		L"          \\        |   /  \\   |       /",
		L"           \\_______|__/____\\__|______/"
	};

	for (int i = 0; i < static_cast<int>(graveyard.size()); ++i)
	{
		Renderer->AddRender(Y + 8 + i, X, graveyard[i], CC_DARKYELLOW);
	}
}

void GameOverUI::RenderMenu(int X, int Y)
{
	static const vector<wstring> leftTexts =
	{
		L"[1]  Retry",
		L"[2]  Quit"
	};

	static const vector<wstring> rightTexts =
	{
		L"다시 도전하기",
		L"메인 메뉴로 가기"
	};

	for (int i = 0; i < 2; ++i)
	{
		const bool bSelected = SelectedIndex == i;
		const int y = Y + i * 4;
		const int color = bSelected ? CC_WHITE : CC_GRAY;

		if (bSelected)
		{
			Renderer->AddRender(y, X - 4, L">", CC_WHITE);
			Renderer->DrawBox(y - 1, X - 1, 76, 3);
		}

		Renderer->AddRender(y, X + 4, leftTexts[i], color);
		Renderer->AddRender(y, X + 44, rightTexts[i], color);
	}
}
