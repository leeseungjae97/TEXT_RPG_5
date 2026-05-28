#pragma once

#include "Monster.h"

class Dragon : public Monster
{
public:
    Dragon(int PlayerLevel);

    //부활테스트
    virtual bool Revive() override;
    virtual void OnSpawnFromPool() override;
    
protected:
    virtual void BuildAttackValue(Player* player) override;
    virtual vector<FItemWeight> GetDropTable() override;
    
private:
    int CurrentPhase = 1;
    //부활테스트
    bool bHasRevive = false;
    
    void UpdatePhase();
    int GetRandomPatternByPhase();
    
    void BreathAttack(Player* player);
    void FireballAttack(Player* player);
    bool RushAttack(Player* player);
    void PhaseThreeAttack(Player* player);
};
