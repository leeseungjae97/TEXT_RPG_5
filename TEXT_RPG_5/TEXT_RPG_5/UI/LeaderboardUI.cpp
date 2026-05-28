#include "LeaderboardUI.h"

#include "../Define.h"
#include "../Manager/DisplayManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/TimeManager.h"
#include "../Network/R2Connector.h"

namespace
{
	string Trim(const string& Text)
	{
		const size_t begin = Text.find_first_not_of(" \t\r\n");
		if (begin == string::npos)
		{
			return "";
		}

		const size_t end = Text.find_last_not_of(" \t\r\n");
		return Text.substr(begin, end - begin + 1);
	}

	string StripRankPrefix(const string& Text)
	{
		const size_t comma = Text.find(',');
		const size_t dot = Text.find('.');
		if (dot != string::npos && comma != string::npos && dot < comma)
		{
			return Trim(Text.substr(dot + 1));
		}

		return Trim(Text);
	}
}

void LeaderboardUI::Open()
{
	Entries.clear();
	bHasLoaded = false;
	DotTimer = 0.0f;
	DotCount = 1;
	StartLoad();
}

LeaderboardAction LeaderboardUI::Tick(float DeltaTime)
{
	StarTimer += DeltaTime;
	if (StarTimer >= 0.35f)
	{
		StarTimer = 0.0f;
		StarOffset = (StarOffset + 1) % 3;
	}

	DotTimer += DeltaTime;
	if (DotTimer >= 0.3f)
	{
		DotTimer = 0.0f;
		DotCount = DotCount % 3 + 1;
	}

	if (bIsLoading && LoadFuture.valid() &&
		LoadFuture.wait_for(chrono::milliseconds(0)) == future_status::ready)
	{
		FinishLoad(LoadFuture.get());
	}

	InputManager* input = InputManager::GetInstance();
	if (input->IsKeyTap(KeyCode::ESCAPE) || input->IsKeyTap(KeyCode::X))
	{
		return LeaderboardAction::Back;
	}

	return LeaderboardAction::None;
}

void LeaderboardUI::Render()
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
	RenderTrophy(SCREEN_WIDTH / 4, 10);
	Renderer->DrawBox(borderY, borderX, borderWidth, borderHeight);

	RenderTitle(28, 8);
	RenderTable(18, 30);

	Renderer->AddRender(SCREEN_HEIGHT - 8, SCREEN_WIDTH / 2 - 18, L"----<>--------------------<>----", CC_GRAY);
	Renderer->AddRender(SCREEN_HEIGHT - 5, SCREEN_WIDTH / 2 - 12, L"Press ESC to Back", CC_GRAY);
	Renderer->AddRender(SCREEN_HEIGHT - 2, SCREEN_WIDTH / 2 - 5, L"Ver. 0.1.0", CC_DARKGRAY);

	if (bIsLoading)
	{
		RenderLoading();
	}
}

void LeaderboardUI::StartLoad()
{
	bIsLoading = true;
	LoadFuture = async(launch::async, []()
	{
		return R2Connector::GetInstance()->ReadLeaderboard();
	});
}

void LeaderboardUI::FinishLoad(const string& Text)
{
	Entries = ParseLeaderboardText(Text);
	bIsLoading = false;
	bHasLoaded = true;
}

vector<LeaderboardEntry> LeaderboardUI::ParseLeaderboardText(const string& Text)
{
	vector<LeaderboardEntry> result;
	stringstream stream(Text);
	string line;

	while (getline(stream, line))
	{
		line = StripRankPrefix(line);
		if (line.empty())
		{
			continue;
		}

		vector<string> tokens;
		string token;
		stringstream lineStream(line);
		while (getline(lineStream, token, ','))
		{
			tokens.push_back(Trim(token));
		}

		if (tokens.size() < 3)
		{
			continue;
		}

		LeaderboardEntry entry;
		try
		{
			entry.TimeSeconds = stod(tokens[0]);
			entry.Level = stoi(tokens[1]);
		}
		catch (...)
		{
			continue;
		}

		entry.Name = DisplayManager::GetInstance()->ToWideString(tokens[2]);
		result.push_back(entry);
	}

	sort(result.begin(), result.end(), [](const LeaderboardEntry& Left, const LeaderboardEntry& Right)
	{
		return Left.TimeSeconds < Right.TimeSeconds;
	});

	for (int i = 0; i < static_cast<int>(result.size()); ++i)
	{
		result[i].Rank = i + 1;
	}

	if (result.size() > 10)
	{
		result.resize(10);
	}

	return result;
}

void LeaderboardUI::RenderTitle(int X, int Y)
{
	static const vector<wstring> title =
	{
		L" _     _____    _    ____  _____ ____  ____   ___    _    ____  ____  ",
		L"| |   | ____|  / \\  |  _ \\| ____|  _ \\| __ ) / _ \\  / \\  |  _ \\|  _ \\ ",
		L"| |   |  _|   / _ \\ | | | |  _| | |_) |  _ \\| | | |/ _ \\ | |_) | | | |",
		L"| |___| |___ / ___ \\| |_| | |___|  _ <| |_) | |_| / ___ \\|  _ <| |_| |",
		L"|_____|_____/_/   \\_\\____/|_____|_| \\_\\____/ \\___/_/   \\_\\_| \\_\\____/ "
	};

	Renderer->AddRender(Y - 4, X + 44 + StarOffset, L"|", CC_GRAY);
	Renderer->AddRender(Y - 2, X + 20, L"----------------<>----------------", CC_GRAY);
	for (int i = 0; i < static_cast<int>(title.size()); ++i)
	{
		Renderer->AddRender(Y + i, X, title[i], CC_LIGHTGRAY);
	}
	Renderer->AddRender(Y + 7, X + 25, L"----  D I A B L 5  ----", CC_DARKYELLOW);
	Renderer->AddRender(Y + 10, X + 20, L"----------------<>----------------", CC_GRAY);
}

void LeaderboardUI::RenderTable(int X, int Y)
{
	const int width = 118;
	const int height = 26;
	Renderer->DrawBox(Y, X, width, height);

	Renderer->AddRender(Y + 2, X + 4, L"RANK", CC_WHITE);
	Renderer->AddRender(Y + 2, X + 22, L"NAME", CC_WHITE);
	Renderer->AddRender(Y + 2, X + 48, L"LV", CC_WHITE);
	Renderer->AddRender(Y + 2, X + 62, L"TIME", CC_WHITE);
	Renderer->AddRender(Y + 2, X + 84, L"SCORE", CC_WHITE);

	for (int i = 1; i < width - 1; ++i)
	{
		Renderer->PutCell(Y + 4, X + i, L'-', Renderer->MakeConsoleAttribute(CC_GRAY));
	}

	if (bHasLoaded && Entries.empty())
	{
		Renderer->AddRender(Y + 12, X + 34, L"아직 등록된 기록이 없습니다.", CC_DARKGRAY);
		return;
	}

	const int count = min(10, static_cast<int>(Entries.size()));
	for (int i = 0; i < count; ++i)
	{
		const LeaderboardEntry& entry = Entries[i];
		const int rowY = Y + 6 + i * 2;
		const bool bTop = i == 0;
		const int color = bTop ? CC_CYAN : CC_GRAY;
		const int score = max(0, static_cast<int>(100000.0 - entry.TimeSeconds * 10.0 + entry.Level * 100.0));
		wstring name = Renderer->TrimTextToDisplayWidth(entry.Name, 20);

		if (bTop)
		{
			Renderer->AddRender(rowY, X - 4, L">", CC_CYAN);
		}

		Renderer->AddRender(rowY, X + 7, to_wstring(entry.Rank), color);
		Renderer->AddRender(rowY, X + 22, name, color);
		Renderer->AddRender(rowY, X + 49, to_wstring(entry.Level), color);
		Renderer->AddRender(rowY, X + 60, TimeManager::FormatSeconds(entry.TimeSeconds), color);
		Renderer->AddRender(rowY, X + 84, to_wstring(score), color);
	}
}

void LeaderboardUI::RenderTrophy(int X, int Y)
{
static const vector<wstring> deerSkull =
{
    L"                                                                                                                          ::::.",
    L"              .::                                                                                                     --:. :-.--.",
    L"           ..:-=:                   :-.                                                                               .-==-.:-::-:",
    L"        .:=+--+=                  .:=-                                                                .                 -++=::=-:--.",
    L"      .:=+=:-=+:                 .-=+.                                                               .-=:.               -++=- -+--=:",
    L"     :-=++..-=+.                 --+-                                                                 .-+=-.              -*++- :+=--:",
    L"    :=++*- :-++                 :-=+.                                                                   :=+=:              -*+*- -*=-=:",
    L"   :=+**= .-=+=                .--+-                                                                     .+++=              =*+*- =*=-=:",
    L"  .++***: .-=++                :-=+.                                                                      .==+=             .+*+*..**-=-",
    L"  =*###+  .=+++               .--+=                                                                        .+=+=             =*+*+ =#===:",
    L" :*####:  .=++*:              :--+-                                                                         -+=+-            -*++#::#+=+:",
    L" :#%##*   .==+*-             .---+:                                                                          ==++:           :*++#--*+=+-",
    L" -#%%#*    -=+++.            .--=+:                                                                          :==+=           :*++#=+*+++:",
    L" -#%%%#-   :=++*-            :-==+.                                                                          .====.          -***%#*++*-",
    L" .*%%##*:  .=++++            :===+.                                                                           ===+:          ++*##*+**=",
    L"  .*%%###=. -++**-           :==++.                                                                           =+==-         -++***+*#=",
    L"    =#%%##*=:=+***:          :===+:                                                        ..                 ===+-        -+++++*#*-",
    L"     .=#%####****#*:         .===+=                  :=:                                   -=.               .+-=+-      :=+++**##=.",
    L"       .=*%%#*******=:        ====+:                .=+:                                   -=-               ==-=+.   .-==++**#*=.",
    L"          :=*###*****++-:..   :====+.               :=+=                                  .=--             .-+=+++:--=+++*##*+-",
    L"             .-=*###***++++==-===+==+:.             :==+.                                 -+-=.       ..::-=======+++****+=:",
    L"                 .:-+*******++++=+=====-::...       :+=+-                                :++===--------=-=====++****+=-:",
    L"                       .:-=++****+++++=========----:-++++=                             :=****+=======+++****++==-:.",
    L"                             .::-==+++++*+++++++=====+++**+-                         :=******+++****++==-::.",
    L"                                      .::--==++********####*+-.                    -++****##%#*=-:.",
    L"                                               ..-=+*#%%##%##**=:                :+#**+*#%@#+:",
    L"                                                    .:=*%%%%%####+::.   .......:=*#***%%%#=.",
    L"                                                        :+%%%%#*+====----=--------=+*#@%+:",
    L"                                                          .+%%+---==-----=------:---=*#=",
    L"                                                            -**++---===-=+----=---+***-",
    L"                                                             :=*#+--=----=-----=-=##*:",
    L"                                                               =+*-=#+-====---=#=-+*=",
    L"                                                              :=++-=**---==---+*+--=+-.",
    L"                                                            :+====-+*+----==-==++=---*%*-",
    L"                                                           :#@%+==-===-----==-====--=%@@#:",
    L"                                                           -#@@@+--===---==--------+#%%%#-",
    L"                                                           :*#%@@**+==-====-==-=+*######*:",
    L"                                                            -*#%#%#%*+====+--=+*%%#%%##*-",
    L"                                                             :+###*#%*=-==+====**%**##*:",
    L"                                                               =#**##*====+=-==*###**#=",
    L"                                                               .**###*====*===+***#***-",
    L"                                                                +****#+===+==++#**#***-",
    L"                                                                =*+**+*+==+==+++*##**+.",
    L"                                                                :****+++==*==+*++****=",
    L"                                                                 +**++++==*+=+*++*+*+.",
    L"                                                                 .***+*+==++++*++**=.",
    L"                                                                  .****+++*+++*+**+",
    L"                                                                   -*+##*+++=##+**:",
    L"                                                                   .**%@#%%@*%@**+.",
    L"                                                                    =*%@=:%@-:%#*=",
    L"                                                                    -*%#  ##. *#*:",
    L"                                                                    .+#+  **. *#*.",
    L"                                                                     -**  +*. **=",
    L"                                                                     .+#=-++==**:",
    L"                                                                      -***+++++=",
    L"                                                                       -*++*++=.",
    L"                                                                        .. .."
};

	for (int i = 0; i < static_cast<int>(deerSkull.size()); ++i)
	{
		Renderer->AddRender(Y + i, X + 15, deerSkull[i], CC_LIGHTGRAY);
	}
}

void LeaderboardUI::RenderLoading()
{
	const int width = 42;
	const int height = 7;
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
	wstring dots(DotCount, L'.');
	wstring text = L"순위 불러오는중" + dots;
	Renderer->AddRender(y + 3, x + (width - Renderer->GetTextDisplayWidth(text)) / 2, text, CC_CYAN);
}
