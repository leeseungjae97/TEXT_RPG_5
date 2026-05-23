#include "InputManager.h"
#include "pch.h"

int ASCII[(UINT)eKeyCode::END] =
{
    '1', '2', '3', '4',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M',
    VK_UP, VK_DOWN, VK_LEFT ,VK_RIGHT
};

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}
void InputManager::BeginPlay()
{
    Keys.resize((UINT)eKeyCode::END, false);
}

void InputManager::Tick(float DeltaTime)
{
    for (int i = 0; i < (UINT)eKeyCode::END; ++i)
    {
        if (GetAsyncKeyState(ASCII[i]) & 0x8000)
        {
            Keys[i] = true;
        }
        else
            Keys[i] = false;
    }

}