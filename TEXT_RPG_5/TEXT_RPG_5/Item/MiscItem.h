// MiscItem.h

#pragma once


#include "Item.h"

class MiscItem : public UItem
{
public:
    MiscItem(const FItemInfo& Info) : UItem(Info){}
    ~MiscItem() {}
};
