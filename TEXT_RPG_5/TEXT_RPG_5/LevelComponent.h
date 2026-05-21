#pragma once
#include "Component.h";
class Player;

class LevelComponent : public Component
{
public:
    Player* player;
    LevelComponent(Player* player) : player(player) {}

    const int MaxLevel = 10;
    void StatsUpdate();

};