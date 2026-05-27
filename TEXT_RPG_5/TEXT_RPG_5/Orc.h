#pragma once

#include "Monster.h"

class Orc : public Monster
{
public:
	Orc(int PlayerLevel);
	
protected:
	virtual void Attackplayer(Player* player) override;
	virtual vector<FItemWeight> GetDropTable() override;
};
