#pragma once
#include <vector>


struct Direction
{
	int X, Y;
	Direction(int X = 1, int Y = 0) : X(X), Y(Y) {}


	void operator+=(const Direction& other)
	{
		X += other.X;
		Y += other.Y;
	}

	Direction operator+(const Direction& other)
	{
		return Direction(X + other.X, Y + other.Y);
	}

	Direction operator*(const int& mul)
	{
		return Direction(X * mul, Y * mul);
	}
	
	Direction operator*=(const int& mul)
	{
		return Direction(X * mul, Y * mul);
	}

	void operator=(const Direction & other)
	{
		X = other.X;
		Y = other.Y;
	}

	bool operator==(const Direction& other) const
	{
		return (X == other.X) && (Y == other.Y);
	}
};
