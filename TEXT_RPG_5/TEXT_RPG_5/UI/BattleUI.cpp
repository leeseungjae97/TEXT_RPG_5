#include "BattleUI.h"

#include "../Define.h"
#include "../Manager/BattleManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/DisplayManager.h"

void BattleUI::Open()
{
	bOpen = true;
}

void BattleUI::Close()
{
	bOpen = false;
}

void BattleUI::Tick(float DeltaTime)
{
	if (!bOpen)
	{
		return;
	}

	if (InputManager::GetInstance()->IsKeyTap(KeyCode::ESCAPE))
	{
		Close();
	}
}

void BattleUI::Render()
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

	const int width = 44;
	const int height = 18;
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
	Renderer->AddRender(y + 1, x + 2, L"잡은 몬스터");
	Renderer->AddRender(y + 1, x + width - 10, L"(ESC) 닫기");

	const vector<pair<string, int>>& killedMonsters = BattleManager::GetInstance()->GetKilledMonsters();
	if (killedMonsters.empty())
	{
		Renderer->AddRender(y + 4, x + 2, L"아직 잡은 몬스터가 없습니다.");
		return;
	}

	int line = 0;
	for (const pair<string, int>& monster : killedMonsters)
	{
		if (line >= height - 5)
		{
			break;
		}

		wstring name = Renderer->ToWideString(monster.first);
		Renderer->AddRender(y + 3 + line, x + 3, name + L" x " + to_wstring(monster.second));
		++line;
	}
}
