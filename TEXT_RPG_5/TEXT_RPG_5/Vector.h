#pragma once

struct Vector
{
public:
	Vector() : X(0), Y(0) {}
	Vector(int _X, int _Y) : X(_X), Y(_Y) {}

public:
	int X, Y;
};
