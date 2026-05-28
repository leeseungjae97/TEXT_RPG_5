//RarityColor.h  (UI 전용: 등급 -> 표시 색)

#pragma once

#include "../Enum/RarityEnum.h"
#include "../Manager/DisplayManager.h"

inline int GetRarityColor(ERarity Rarity)
{
	switch (Rarity)
	{
	case ERarity::Rare:   return CC_CYAN;
	case ERarity::Unique: return CC_MAGENTA;
	case ERarity::Common:
	default:              return CC_WHITE;
	}
}
