#pragma once

#include "Monster.h"

class Dragon : public Monster
{
public:
    Dragon(int PlayerLevel);
    
protected:
    virtual void BuildAttackValue(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;
};
