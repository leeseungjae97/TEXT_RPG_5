#pragma once
#include "../Singleton.h"
#include "../pch.h"

enum class KeyCode
{
	_1, _2, _3, _4,
	Z, X, C, V, B, N, M,
	UP, DOWN, LEFT, RIGHT, ESCAPE, END
};

enum class KeyState
{
	Tap,
	Pressed,
	Released,
	None,
	MAX
};

struct KeyInfo
{
	KeyState State;
	bool bPrev;
};

class InputManager : public Singleton<InputManager>
{
public:
	InputManager();
	~InputManager();

public:
	void Tick(float DeltaTime);
	void BeginPlay();

private:
	vector<KeyInfo> Keys;

public:
	bool IsKeyPressed	(KeyCode KeyCode) { return Keys[(UINT)KeyCode].State == KeyState::Pressed;	}
	bool IsKeyTap		(KeyCode KeyCode) { return Keys[(UINT)KeyCode].State == KeyState::Tap;		}
	bool IsKeyReleased	(KeyCode KeyCode) { return Keys[(UINT)KeyCode].State == KeyState::Released; }
	KeyState GetKeyState(KeyCode KeyCode) { return Keys[(UINT)KeyCode].State; }
};
