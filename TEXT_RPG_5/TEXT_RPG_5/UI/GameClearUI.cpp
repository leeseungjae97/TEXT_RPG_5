#include "GameClearUI.h"

#include "../Define.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/TimeManager.h"

GameClearAction GameClearUI::Tick(float DeltaTime)
{
	StarTimer += DeltaTime;
	if (StarTimer >= 0.35f)
	{
		StarTimer = 0.0f;
		StarOffset = (StarOffset + 1) % 3;
	}

	InputManager* input = InputManager::GetInstance();
	if (input->IsKeyTap(KeyCode::UP) || input->IsKeyTap(KeyCode::DOWN))
	{
		SelectedIndex = input->IsKeyTap(KeyCode::UP) ? (SelectedIndex + 2) % 3 : (SelectedIndex + 1) % 3;
	}

	if (input->IsKeyTap(KeyCode::_1))
	{
		return GameClearAction::Retry;
	}
	if (input->IsKeyTap(KeyCode::_2) || input->IsKeyTap(KeyCode::ESCAPE))
	{
		return GameClearAction::MainMenu;
	}
	if (input->IsKeyTap(KeyCode::_3))
	{
		return GameClearAction::RegisterRank;
	}
	if (input->IsKeyTap(KeyCode::Z))
	{
		if (SelectedIndex == 0) return GameClearAction::Retry;
		if (SelectedIndex == 1) return GameClearAction::MainMenu;
		return GameClearAction::RegisterRank;
	}

	return GameClearAction::None;
}

void GameClearUI::Render()
{
	if (Renderer == nullptr)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

	RenderTitle(10, 16);
	RenderCastle(SCREEN_WIDTH / 2 + 12, 14);
	RenderMenu(54, 36);
	RenderEpilogue(4, SCREEN_HEIGHT - 13);
	Renderer->AddRender(31, 54, L"CLEAR TIME : " + TimeManager::GetInstance()->GetFormattedStoredTime(), CC_CYAN);
}

void GameClearUI::RenderTitle(int X, int Y)
{
	static const vector<wstring> title =
	{
		L"  ____    _    __  __ _____    ____ _     _____    _    ____  ",
		L" / ___|  / \\  |  \\/  | ____|  / ___| |   | ____|  / \\  |  _ \\ ",
		L"| |  _  / _ \\ | |\\/| |  _|   | |   | |   |  _|   / _ \\ | |_) |",
		L"| |_| |/ ___ \\| |  | | |___  | |___| |___| |___ / ___ \\|  _ < ",
		L" \\____/_/   \\_\\_|  |_|_____|  \\____|_____|_____/_/   \\_\\_| \\_\\"
	};

	Renderer->AddRender(Y - 2, X - 4, L"---<>", CC_GRAY);
	for (int i = 0; i < static_cast<int>(title.size()); ++i)
	{
		Renderer->AddRender(Y + i, X, title[i], CC_LIGHTGRAY);
	}
	Renderer->AddRender(Y + 7, X + 14, L"---- D I A B L 5  ----", CC_DARKYELLOW);
}

void GameClearUI::RenderCastle(int X, int Y)
{
	static const vector<wstring> castle =
	{
		L"                         |",
		L"                        / \\",
		L"             /\\        /___\\        /\\",
		L"            /__\\     ___| |___     /__\\",
		L"        /\\   ||    _|  [] [] |_    ||   /\\",
		L"       /__\\ _||___|_|________|_|___||_ /__\\",
		L"        || | [] [] [] [] [] [] [] [] |  ||",
		L"     ___||_|_________________________|__||___",
		L"    /______/|  |  |  |  |  |  |  |\\______\\",
		L"    | [] [] |__|__|__|__|__|__|__| [] [] |",
		L"    |_______|  |        /\\        |_______|",
		L"       ||      |       /__\\       |      ||",
		L"       ||______|________||________|______||",
		L"          | | | | | | | || | | | | | |",
		L"          | | | | | | | || | | | | | |",
			L" ",
		L"Render, Engine, UI : 이승재",
		L"Player, Weapon : 박성규",
		L"Inventory, Item, Shop : 김인철",
		L"Monster, Attack Pattern : 최현준",
		L"LevelUp 성창훈",
		L"DIABL5 - Console Isometric RPG",
	};

	for (int i = 0; i < static_cast<int>(castle.size()); ++i)
	{
		Renderer->AddRender(Y + i, X + ((i + StarOffset) % 2), castle[i], CC_GRAY);
	}
}

void GameClearUI::RenderMenu(int X, int Y)
{
	static const vector<wstring> leftTexts = { L"[1]  Retry", L"[2]  Quit", L"[3]  Register" };
	static const vector<wstring> rightTexts = { L"다시 플레이하기", L"메인 메뉴로 가기", L"랭크 등록" };

	for (int i = 0; i < 3; ++i)
	{
		const bool bSelected = SelectedIndex == i;
		const int y = Y + i * 4;
		const int color = bSelected ? CC_WHITE : CC_GRAY;
		if (bSelected)
		{
			Renderer->AddRender(y, X - 4, L">", CC_CYAN);
			Renderer->DrawBox(y - 1, X - 1, 66, 3);
		}

		Renderer->AddRender(y, X + 4, leftTexts[i], color);
		Renderer->AddRender(y, X + 36, rightTexts[i], bSelected ? CC_CYAN : color);
	}
}

void GameClearUI::RenderEpilogue(int X, int Y)
{
	Renderer->AddRender(Y, X, L"YOU HAVE DESCENDED INTO THE DEPTHS OF DIABL5.", CC_GRAY);
	Renderer->AddRender(Y + 2, X, L"THE LORDS OF DARKNESS HAVE BEEN SILENCED.", CC_GRAY);
	Renderer->AddRender(Y + 4, X, L"YOUR LEGEND WILL ECHO THROUGH THE ABYSS.", CC_GRAY);
	Renderer->AddRender(Y + 7, X, L"THANK YOU, WANDERER.", CC_GRAY);
	Renderer->AddRender(Y + 9, X, L"-----------------------------<>-----------------------------", CC_GRAY);
}
