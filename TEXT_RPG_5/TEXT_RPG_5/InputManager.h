#pragma once
#include "Singleton.h"
#include "pch.h"

enum class eKeyCode
{
	_1, _2, _3, _4,
	Z, X, C, V, B, N, M,
	UP, DOWN, LEFT, RIGHT, END
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
	vector<bool> Keys;

public:
	bool IsKeyDown(eKeyCode KeyCode) { return Keys[(UINT)KeyCode]; }
};