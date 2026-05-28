#pragma once

#include "Monster.h"

enum class EOrcMageAttackPattern
{
    Star,
    Spiral
};

class OrcMage : public Monster
{
public:
    OrcMage(int PlayerLevel);

protected:
    virtual void BuildAttackValue(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;

private:
    int ExplosionRadius = 0;     
    int PatternScale = 2;
    
private:
    void AddAttackCell(Vector Pos);
    void AddPattern(Vector Center, const vector<Vector>& Offsets);

    void BuildStarPattern(Vector Center);
    void BuildSpiralPattern(Vector Center);
    
};
