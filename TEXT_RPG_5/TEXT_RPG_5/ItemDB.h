#pragma once
#include "Item.h"

namespace ItemDB
{
	const FItemInfo HP_POTION = { "HP포션", ItemType::Usable, ItemId::HP_POTION , 50, 50 };
	const FItemInfo STRENGTH_POTION = { "힘 포션", ItemType::Usable, ItemId::STRENGTH_POTION , 50, 10 };
	const FItemInfo GOBLIN_LEATHER = { "고블린 가죽", ItemType::Misc, ItemId::GOBLIN_LEATHER , 0, 0 };
}
