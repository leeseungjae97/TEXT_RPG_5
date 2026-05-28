#pragma once

#include "Monster.h"

class Mimic : public Monster
{
public:
    Mimic(int PlayerLevel);

protected:
    virtual void BuildAttackValue(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;

private:
    void RushWithCrossAttack();
    void CrossRushAttack();
    void AddAttackCell(Vector Pos);
    
public:
    bool IsDiscovered() { return bIsDiscovered; }
    void SetDiscovered() { bIsDiscovered = true; }
    
private:
    bool bIsDiscovered;
};
