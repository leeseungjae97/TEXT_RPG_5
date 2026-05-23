#pragma once
#include "Component.h"
#include "ComponentTypeEnum.h"
class AObject;

class SubActionComponent : public UComponent
{
    friend class AObject;

public:
    static constexpr ComponentType Type = ComponentType::SubActionComponent;

    virtual void Tick(float DeltaTime) override;
    void UseSlotItem(int NumKey);

private:
    SubActionComponent(AObject* InOwner) : UComponent(InOwner) {}
};
