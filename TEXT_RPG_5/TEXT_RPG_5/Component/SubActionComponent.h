#pragma once
#include "Component.h"
#include "../Enum/ComponentTypeEnum.h"
class AObject;

class USubActionComponent : public UComponent
{
    friend class AObject;

protected:
    USubActionComponent(AObject* InOwner) : UComponent(InOwner) {}
    USubActionComponent() = delete;

public:
    static constexpr ComponentType Type = ComponentType::SubActionComponent;
    virtual void Tick(float DeltaTime) override;
    void UseSlotItem(int NumKey);
private:
    float const CoolTime= 3.0f;
    float TimeCheck = 3.0f;
};
