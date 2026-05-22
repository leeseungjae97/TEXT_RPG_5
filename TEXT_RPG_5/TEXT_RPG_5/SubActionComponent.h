#pragma once
#include "Component.h"
class AObject;

class SubActionComponent : public UComponent
{
public:
    SubActionComponent(AObject* InOwner) : UComponent(InOwner) {}

    void Tick(float DeltaTime);
    void UseSlotItem(int NumKey);
};