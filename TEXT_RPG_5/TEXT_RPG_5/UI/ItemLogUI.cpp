#include "ItemLogUI.h"

#include "../Item/Item.h"
#include "../Manager/DisplayManager.h"
#include "../Define.h"

void ItemLogUI::AddItemLog(const UItem* Item)
{
	if (Item == nullptr)
	{
		return;
	}

	DisplayManager* renderer = DisplayManager::GetInstance();
	wstring itemName = renderer->ToWideString(Item->GetItemInfo().Name);
	AddLog(itemName + L" 획득");
}

void ItemLogUI::AddLog(const wstring& Text)
{
	if (Text.empty())
	{
		return;
	}

	Logs.push_back({ Text, LogDuration });
	while (static_cast<int>(Logs.size()) > MaxLogCount)
	{
		Logs.erase(Logs.begin());
	}
}

void ItemLogUI::Reset()
{
	Logs.clear();
}

void ItemLogUI::Tick(float DeltaTime)
{
	for (FItemLogEntry& log : Logs)
	{
		log.RemainTime -= DeltaTime;
	}

	Logs.erase(
		remove_if(Logs.begin(), Logs.end(), [](const FItemLogEntry& log)
			{
				return log.RemainTime <= 0.0f;
			}),
		Logs.end());
}

void ItemLogUI::Render()
{
	if (Logs.empty())
	{
		return;
	}

	if (Renderer == nullptr)
	{
		Renderer = DisplayManager::GetInstance();
		return;
	}

	const int rightPadding = 4;
	const int bottomY = SCREEN_HEIGHT - 14;
	const int startY = max(1, bottomY - static_cast<int>(Logs.size()) + 1);

	for (int i = 0; i < static_cast<int>(Logs.size()); ++i)
	{
		wstring text = Renderer->TrimTextToDisplayWidth(Logs[i].Text, 32);
		const int textWidth = Renderer->GetTextDisplayWidth(text);
		const int x = max(1, SCREEN_WIDTH - rightPadding - textWidth);
		Renderer->AddRender(startY + i, x, text, CC_WHITE);
	}
}
