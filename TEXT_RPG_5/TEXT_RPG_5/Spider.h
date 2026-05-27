#pragma once

#include "Monster.h"

class Spider : public Monster
{
public: 
    Spider(int PlayerLevel);
    
protected:
    virtual void Attackplayer(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;
};
