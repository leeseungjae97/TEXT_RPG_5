#pragma once

#include "Monster.h"

class QueenSpider : public Monster
{
public:
    QueenSpider(int PlayerLevel);

protected:
    virtual void BuildAttackValue(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;

private:
    bool RushAttack(Player* player);
    void WebAreaAttack(Player* player);

    void AddAttackCell(Vector Pos);
};
