#pragma once

//simple x y vertex with op ovrld so it may fit in a std::set. Used for determining grid pos of entities
struct GridVertex
{
	int row, col;

	GridVertex() {}
	GridVertex(int col = 0, int row = 0) : row(row), col(col) {}

	//(2, 4) is  < (4, 1)
	bool operator<(const GridVertex& other) const
	{
		if (row == other.row)
			return (col < other.col);

		return (row < other.row);
	}

	bool operator==(const GridVertex& other) const
	{
		return (row == other.row) && (col == other.col);
	}

	bool operator!=(const GridVertex& other) const
	{
		return (row != other.row) && (col != other.col);
	}

	void operator=(const GridVertex& other)
	{
		row = other.row;
		col = other.col;
	}

	GridVertex operator+(const GridVertex& other) const
	{
		return GridVertex(col + other.col, row + other.row);
	}
};

