#pragma once


class AObject;
class UComponent
{
public:
    virtual ~UComponent() = default;
    UComponent() = delete;

    AObject* GetOwner() { return Owner; }
    void SetOwner(AObject* InOwner) { Owner = InOwner; }

public:
    virtual void Tick(float DeltaTime) = 0;

private:
    AObject* Owner;

protected:
    UComponent(AObject* InOwner);
};
