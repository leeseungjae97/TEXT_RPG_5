#pragma once
#include "Item.h"

namespace ItemDB
{
	const FItemInfo HP_POTION       = { "HP포션",      ItemType::Usable,    ItemId::HP_POTION,       50,   50,  StatType::HP,    EquipmentType::NONE   };
	const FItemInfo STRENGTH_POTION = { "힘 포션",     ItemType::Usable,    ItemId::STRENGTH_POTION, 50,   10,  StatType::Power, EquipmentType::NONE   };
	const FItemInfo LONGSWORD       = { "롱소드",      ItemType::Equipment, ItemId::LONGSWORD,       200,  10,  StatType::Power, EquipmentType::Weapon, WeaponType::Sword      };
	const FItemInfo BOW             = { "활",          ItemType::Equipment, ItemId::BOW,             150,  8,   StatType::Power, EquipmentType::Weapon, WeaponType::Bow };
	const FItemInfo AXE             = { "도끼",        ItemType::Equipment, ItemId::AXE,             300,  15,  StatType::Power, EquipmentType::Weapon, WeaponType::Axe      };
	const FItemInfo STAFF           = { "지팡이",      ItemType::Equipment, ItemId::STAFF,           250,  12,  StatType::Power, EquipmentType::Weapon, WeaponType::Magic      };
	const FItemInfo GOBLIN_LEATHER  = { "고블린 가죽",  ItemType::Misc,     ItemId::GOBLIN_LEATHER,  80,   0,   StatType::None,  EquipmentType::NONE   };
	const FItemInfo SLIME_JELLY     = { "슬라임 젤리",  ItemType::Misc,     ItemId::SLIME_JELLY,     50,   0,   StatType::None,  EquipmentType::NONE   };
	const FItemInfo ORC_TUSK        = { "오크의 엄니",  ItemType::Misc,     ItemId::ORC_TUSK,        300,  0,   StatType::None,  EquipmentType::NONE   };
	const FItemInfo SPIDER_EYE      = { "거미의 눈",   ItemType::Misc,      ItemId::SPIDER_EYE,      150,  0,   StatType::None,  EquipmentType::NONE   };
	const FItemInfo DRAGON_HEART    = { "용의 심장",   ItemType::Usable,    ItemId::DRAGON_HEART,    1000, 200, StatType::HP,    EquipmentType::NONE   };
	const FItemInfo LEATHER_HELMET  = { "가죽 모자",   ItemType::Equipment, ItemId::LEATHER_HELMET,  100,  25,  StatType::MaxHP, EquipmentType::Head   };
	const FItemInfo LEATHER_ARMOR   = { "가죽 갑옷",   ItemType::Equipment, ItemId::LEATHER_ARMOR,   300,  50,  StatType::MaxHP, EquipmentType::Body   };
	const FItemInfo EXPLORER_BOOTS   = { "탐험가의 부츠",   ItemType::Equipment, ItemId::EXPLORER_BOOTS,   100,  30,  StatType::MovementSpeed, EquipmentType::Boots  };
	const FItemInfo PLATE_HELMET    = { "플레이트 투구", ItemType::Equipment, ItemId::PLATE_HELMET,   400,  60,  StatType::MaxHP, EquipmentType::Head   };
	const FItemInfo PLATE_ARMOR     = { "플레이트 갑옷", ItemType::Equipment, ItemId::PLATE_ARMOR,    900,  120, StatType::MaxHP, EquipmentType::Body   };
	const FItemInfo BOOTS_OF_WIND     = { "바람의 부츠", ItemType::Equipment, ItemId::BOOTS_OF_WIND,    400,  50,  StatType::MovementSpeed, EquipmentType::Boots  };

	const FItemInfo FIRE_WALL_SCROLL     = { "불기둥 주문서",  ItemType::Usable,    ItemId::FIRE_WALL_SCROLL,    120,  80,  StatType::None,  EquipmentType::NONE   };
	const FItemInfo LIGHTNING_STRIKE_SCROLL       = { "전격 주문서", ItemType::Usable,   ItemId::LIGHTNING_STRIKE_SCROLL,      200,  60,  StatType::None,  EquipmentType::NONE   };
	const FItemInfo FLAME_POTION      = { "불의 영약",   ItemType::Usable,   ItemId::FLAME_POTION,     350,  40,  StatType::None,  EquipmentType::NONE   };

	const static FItemInfo ItemDBs[20] = {
		{ "HP포션",       ItemType::Usable,    ItemId::HP_POTION,       50,   50,  StatType::HP,    EquipmentType::NONE   }, //0
		{ "힘 포션",      ItemType::Usable,    ItemId::STRENGTH_POTION, 50,   10,  StatType::Power, EquipmentType::NONE   }, //1
		{ "롱소드",       ItemType::Equipment, ItemId::LONGSWORD,       200,  10,  StatType::Power, EquipmentType::Weapon, WeaponType::Sword      }, //2
		{ "활",           ItemType::Equipment, ItemId::BOW,             150,  8,   StatType::Power, EquipmentType::Weapon, WeaponType::Bow }, //3
		{ "도끼",         ItemType::Equipment, ItemId::AXE,             300,  15,  StatType::Power, EquipmentType::Weapon, WeaponType::Axe      }, //4
		{ "지팡이",       ItemType::Equipment, ItemId::STAFF,           250,  12,  StatType::Power, EquipmentType::Weapon, WeaponType::Magic      }, //5
		{ "고블린 가죽",  ItemType::Misc,      ItemId::GOBLIN_LEATHER,  80,   0,   StatType::None,  EquipmentType::NONE   }, //6
		{ "슬라임 젤리",  ItemType::Misc,      ItemId::SLIME_JELLY,     50,   0,   StatType::None,  EquipmentType::NONE   }, //7
		{ "오크의 엄니",  ItemType::Misc,      ItemId::ORC_TUSK,        300,  0,   StatType::None,  EquipmentType::NONE   }, //8
		{ "거미의 눈",    ItemType::Misc,      ItemId::SPIDER_EYE,      150,  0,   StatType::None,  EquipmentType::NONE   }, //9
		{ "용의 심장",    ItemType::Usable,    ItemId::DRAGON_HEART,    1000, 200, StatType::HP,    EquipmentType::NONE   }, //10
		{ "가죽 모자",    ItemType::Equipment, ItemId::LEATHER_HELMET,  100,  25,  StatType::MaxHP, EquipmentType::Head   }, //11
		{ "가죽 갑옷",    ItemType::Equipment, ItemId::LEATHER_ARMOR,   300,  50,  StatType::MaxHP, EquipmentType::Body   }, //12
		{ "탐험가의 부츠",    ItemType::Equipment, ItemId::EXPLORER_BOOTS,   100,  30,  StatType::MovementSpeed, EquipmentType::Boots  }, //13
		{ "플레이트 투구", ItemType::Equipment, ItemId::PLATE_HELMET,   400,  60,  StatType::MaxHP, EquipmentType::Head   }, //14
		{ "플레이트 갑옷", ItemType::Equipment, ItemId::PLATE_ARMOR,    900,  120, StatType::MaxHP, EquipmentType::Body   }, //15
		{ "바람의 부츠", ItemType::Equipment, ItemId::BOOTS_OF_WIND,    400,  50,  StatType::MovementSpeed, EquipmentType::Boots  }, //16
		{ "불기둥 주문서",  ItemType::Usable,    ItemId::FIRE_WALL_SCROLL,    120,  80,  StatType::None,  EquipmentType::NONE   }, //17
		{ "전격 주문서", ItemType::Usable,   ItemId::LIGHTNING_STRIKE_SCROLL,      200,  60,  StatType::None,  EquipmentType::NONE   }, //18
		{ "불의 영약",   ItemType::Usable,   ItemId::FLAME_POTION,     350,  40,  StatType::None,  EquipmentType::NONE   }, //19
	};
}
