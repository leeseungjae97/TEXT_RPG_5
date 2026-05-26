#include "InputManager.h"
#include "../pch.h"

int ASCII[(UINT)KeyCode::END] =
{
    '1', '2', '3', '4',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M',
    VK_UP, VK_DOWN, VK_LEFT ,VK_RIGHT, VK_ESCAPE
};

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::BeginPlay()
{
    Keys.resize((UINT)KeyCode::END, {KeyState::None, false});
}

void InputManager::Tick(float DeltaTime)
{
    for (int i = 0; i < (UINT)KeyCode::END; ++i)
    {
        if (GetAsyncKeyState(ASCII[i]) & 0x8000)
        {
            if (false == Keys[i].bPrev)
            {
                Keys[i].State = KeyState::Tap;
                Keys[i].bPrev = true;
            }
            else
            {
                Keys[i].State = KeyState::Pressed;
            }
        }
        else
        {
            if (false == Keys[i].bPrev)
            {
                Keys[i].State = KeyState::None;
            }
            else
            {
                Keys[i].State = KeyState::Released;
                Keys[i].bPrev = false;
            }
        }
    }
}
