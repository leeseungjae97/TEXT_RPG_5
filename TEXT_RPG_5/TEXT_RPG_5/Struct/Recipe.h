//Recipe.h

#pragma once

#include "../pch.h"
#include "../Enum/ItemIdEnum.h"

struct FRecipe
{
    ItemId Result;
    vector<ItemId> Ingredients;   // 2개 이상. 같은 Id를 중복해서 넣으면 그 개수만큼 필요
};
