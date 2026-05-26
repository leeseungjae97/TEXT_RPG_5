#pragma once
#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
class AObject;

class SubActionComponent : public UComponent
{
    friend class AObject;

protected:
    SubActionComponent(AObject* InOwner) : UComponent(InOwner) {}
    SubActionComponent() = delete;

public:
    static constexpr ComponentType Type = ComponentType::SubActionComponent;
    virtual void Tick(float DeltaTime) override;
    void UseSlotItem(int NumKey);
private:
    float const CoolTime= 3.0f;
    float TimeCheck = 3.0f;
};
