#pragma once
#include "Component.h";
class AObject;

class LevelComponent : public UComponent
{
public:
    LevelComponent(AObject* InOwner) : UComponent(InOwner) {}

    const int MaxLevel = 10;    
    void AddExp(int InExp);
    void StatsUpdate(int LevelUpCount);

};