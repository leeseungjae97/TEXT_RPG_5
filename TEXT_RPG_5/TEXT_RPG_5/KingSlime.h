#pragma once

#include "Monster.h"

class KingSlime : public Monster
{
public:
    KingSlime(int Playerlevel);
    
protected:
    virtual void Attackplayer(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;
    
private:
    void GroundAttack(Player* player);
    void CrossAttack(Player* player);
};
