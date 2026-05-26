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

struct FVector
{
public:
	FVector() : X(0.f), Y(0.f) {}
	FVector(float _X, float _Y) : X(_X), Y(_Y) {}
	FVector(const FVector& Other) : X(Other.X), Y(Other.Y) {}

public:
	float X, Y;
	bool operator!=(const FVector& vector) const
	{
		return X != vector.X || Y != vector.Y;
	}
};
