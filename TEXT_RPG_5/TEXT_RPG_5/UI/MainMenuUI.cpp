#include "MainMenuUI.h"

#include "../Define.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/InputManager.h"

MainMenuAction MainMenuUI::Tick(float DeltaTime)
{
	StarTimer += DeltaTime;
	if (StarTimer >= 0.35f)
	{
		StarTimer = 0.0f;
		StarOffset = (StarOffset + 1) % 3;
	}

	InputManager* input = InputManager::GetInstance();
	if (bShowCredits)
	{
		if (input->IsKeyTap(KeyCode::ESCAPE) || input->IsKeyTap(KeyCode::X) || input->IsKeyTap(KeyCode::Z))
		{
			bShowCredits = false;
		}
		return MainMenuAction::None;
	}

	if (input->IsKeyTap(KeyCode::UP))
	{
		SelectedIndex = (SelectedIndex + 3) % 4;
	}
	if (input->IsKeyTap(KeyCode::DOWN))
	{
		SelectedIndex = (SelectedIndex + 1) % 4;
	}

	if (input->IsKeyTap(KeyCode::_1))
	{
		return MainMenuAction::StartGame;
	}
	if (input->IsKeyTap(KeyCode::_2))
	{
		bShowCredits = true;
		return MainMenuAction::None;
	}
	if (input->IsKeyTap(KeyCode::_3))
	{
		return MainMenuAction::Leaderboard;
	}
	if (input->IsKeyTap(KeyCode::_4) || input->IsKeyTap(KeyCode::ESCAPE))
	{
		return MainMenuAction::Exit;
	}
	if (input->IsKeyTap(KeyCode::Z))
	{
		if (SelectedIndex == 0) return MainMenuAction::StartGame;
		if (SelectedIndex == 1)
		{
			bShowCredits = true;
			return MainMenuAction::None;
		}
		if (SelectedIndex == 2) return MainMenuAction::Leaderboard;
		return MainMenuAction::Exit;
	}

	return MainMenuAction::None;
}

void MainMenuUI::Render()
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

	RenderTitle(10, 12);
	RenderLandscape(SCREEN_WIDTH / 2 + 8, 6);
	RenderMenu(34, 32);

	if (bShowCredits)
	{
		RenderCredits(34, 46);
	}
}

void MainMenuUI::RenderTitle(int X, int Y)
{
	static const vector<wstring> title =
	{
		L" ____   ___    _    ____  _     ____  ",
		L"|  _ \\ |_ _|  / \\  | __ )| |   | ___| ",
		L"| | | | | |  / _ \\ |  _ \\| |   |___ \\ ",
		L"| |_| | | | / ___ \\| |_) | |___ ___) |",
		L"|____/ |___/_/   \\_\\____/|_____|____/ ",
		L"",
		L"R E A L - T I M E   I S O M E T R I C   T E X T   R P G"
	};

	for (int i = 0; i < static_cast<int>(title.size()); ++i)
	{
		Renderer->AddRender(Y + i, X + 30, title[i], i == 6 ? CC_DARKYELLOW : CC_LIGHTGRAY);
	}
}

void MainMenuUI::RenderLandscape(int X, int Y)
{
	static const vector<wstring> sky =
	{
		L"          .       *             .             *                 .          *",
		L"    .                 .                  .               .                  ",
		L"              .              *                    .                     .   ",
		L"       *              .              .                   *           *      "
	};

	for (int i = 0; i < static_cast<int>(sky.size()); ++i)
	{
		Renderer->AddRender(Y + ((i + StarOffset) % 3), X + i, sky[i], CC_DARKYELLOW);
	}

static const vector<wstring> skull =
{
    L"==---++**+                               :.                       +***+---=--::.",
    L"==--=++**-                         ....  ::                       -***+=-:---::.",
    L"===-==++*.                       :---:-::--.                       =**++-==--::.",
    L"=+++=++++.                  .:::.:......    .:...  ..              :**+++++=-::.",
    L"+====++++               :-:-:::...           ::--:---:. .          .**++++=+=:..",
    L"+====++*=           ...::..                     ...::::..           =#*++==+=-:.",
    L"==-=+++*.          .:..                               .....         .##*+=====-.",
    L"--=++++:       ...                                        ...:.      =%#*+===-:.",
    L"===+*+.     :.::.                            ..              ....     +##*+==--",
    L"-=+**=     ..      .:--=+=-----------:..::-=====+++*+=---:             +#*+==-:",
    L"=++**.           :=-.:-+*%#*%%#*+***+=--*####%%@@@#+-.....=-     ..     -*++==:.",
    L"++++.      .     ==     .===#@@@@@%#*-+%@@@@@@%*-..--     +=    ...      -**+=-.",
    L"+++.      ..     :*     .*+. :=+#%%*==++++**=:    -*:     ++.    ..      .**+=-.",
    L"+*:       ..    .=#                  =#*===               :%+:   .        +*++-.",
    L"+*.       .    :=+%: ....       -+::=@@@%%@#=        .... =%*:    .       .+*+=.",
    L"**-       :    .+*@%+*****++==-*@@+:+%@@%%@@@%=:::=+=+*#*+%%+-    .        :**=.",
    L"**-       :     -*%@@@@%@@@@@@@@@@#+*%@@#%@@@@@@%#%@%@@@@%%#+.    .         +*=.",
    L"**-       .     :#%%@%@@@@@@@@@%%%##--+:-%@@@@@@@@@@@@%@@%%%-       .      :+*=.",
    L"+=.              -+%@%*+*%@#**#%%%%+     =%@@@%#*#%#+==+#%#-        .      .++=.",
    L".                  .:    :+%++#%@@#   :.  =@%@@%+-.                         ...",
    L"       .                   ...=%@%#=-=*+++#%##%=..                              ",
    L"      ..                    .===+=+*##**#*####*=-                               ",
    L"     .                       .-=-=+*##*++***##=:                                ",
    L"     .                         ::---:=-:-----:.             .                   ",
    L"   ..              .             .:+:*+##-:                    . .      ..      ",
    L"   .           ......             .:.-:==-..     .                              ",
    L"          . ..                     .-==---:.                      .::.          ",
    L"                                   .:=**=:...                                    ",
    L"                                .=+**#@@@%*++.                                  ",
    L"                                -#@@##@@@%@@+:                                  ",
    L"                                :=*%#*+#*##*+:                                  ",
    L"                                 .:=-    .::.                                   "
};

	for (int i = 0; i < static_cast<int>(skull.size()); ++i)
	{
		Renderer->AddRender(Y + 8 + i, X, skull[i], CC_DARKYELLOW);
	}
}

void MainMenuUI::RenderMenu(int X, int Y)
{
	static const vector<wstring> leftTexts =
	{
		L"[1]  Game Start",
		L"[2]  Credits",
		L"[3]  Leaderboard",
		L"[4]  Exit"
	};

	static const vector<wstring> rightTexts =
	{
		L"새 게임 시작",
		L"제작진",
		L"리더보드",
		L"게임 종료"
	};

	for (int i = 0; i < 4; ++i)
	{
		const bool bSelected = SelectedIndex == i;
		const int y = Y + i * 4;
		const int color = bSelected ? CC_WHITE : CC_GRAY;

		if (bSelected)
		{
			Renderer->AddRender(y, X - 4, L">", CC_WHITE);
			Renderer->DrawBox(y - 1, X - 1, 72, 3);
		}

		Renderer->AddRender(y, X + 4, leftTexts[i], color);
		Renderer->AddRender(y, X + 48, rightTexts[i], color);
	}

	Renderer->AddRender(Y + 15, X + 4, L"방향키 선택 / Z 확인 / ESC 종료", CC_DARKGRAY);
}

void MainMenuUI::RenderCredits(int X, int Y)
{
	const int width = 64;
	const int height = 12;
	Y += 4;
	Renderer->DrawBox(Y, X, width, height);
	Renderer->AddRender(Y + 2, X + 4, L"Credits", CC_WHITE);
	Renderer->AddRender(Y + 4, X + 4, L"Render, Engine, UI : 이승재", CC_GRAY);
	Renderer->AddRender(Y + 5, X + 4, L"Player, Weapon : 박성규", CC_GRAY);
	Renderer->AddRender(Y + 6, X + 4, L"Inventory, Item, Shop : 김인철", CC_GRAY);
	Renderer->AddRender(Y + 7, X + 4, L"Monster, Attack Pattern : 최현준", CC_GRAY);
	Renderer->AddRender(Y + 8, X + 4, L"LevelUp 성창훈", CC_GRAY);
	Renderer->AddRender(Y + 10, X + 4, L"DIABL5 - Console Isometric RPG", CC_DARKYELLOW);
	Renderer->AddRender(Y + 12, X + 4, L"(ESC/Z/X) 돌아가기", CC_DARKGRAY);
}
