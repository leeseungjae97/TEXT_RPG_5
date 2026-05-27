#pragma once

#include "Monster.h"

class Dragon : public Monster
{
public:
    Dragon(int PlayerLevel);
    
protected:
    virtual void Attackplayer(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;
};
