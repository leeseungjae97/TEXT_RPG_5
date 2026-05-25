#pragma once

struct Vector
{
public:
	Vector() : X(0), Y(0) {}
	Vector(int _X, int _Y) : X(_X), Y(_Y) {}
	Vector(const Vector& Other) : X(Other.X), Y(Other.Y) {}

public:
	int X, Y;
	bool operator!=(const Vector& vector) const
	{
		return X != vector.X || Y != vector.Y;
	}
};
