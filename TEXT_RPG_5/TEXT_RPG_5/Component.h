#pragma once

class AObject;
class UComponent
{
public:
    UComponent(AObject* InOwner);
    ~UComponent();
    UComponent() = delete;

    AObject* GetOwner() { return Owner; }
    void SetOwner(AObject* InOwner) { Owner = InOwner; }

public:
    virtual void Tick(float DeltaTime) = 0;

private:
    AObject* Owner;
};