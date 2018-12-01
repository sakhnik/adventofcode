#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>

using namespace std;

int Solve(unsigned rows, string row)
{
	int safe_cells = count(begin(row), end(row), '.');

	auto is_trap = [&](int col)
	{
		if (col < 0 || col >= static_cast<int>(row.size()))
			return false;
		return row[col] == '^';
	};

	const int LEFT = 0x4;
	const int CENTER = 0x2;
	const int RIGHT = 0x1;

	for (unsigned i = 1; i != rows; ++i)
	{
		string new_row;
		for (unsigned j = 0; j != row.size(); ++j)
		{
			int code = is_trap(j-1) ? LEFT : 0;
			if (is_trap(j))
				code |= CENTER;
			if (is_trap(j+1))
				code |= RIGHT;
			switch (code)
			{
			case LEFT | CENTER:
			case CENTER | RIGHT:
			case LEFT:
			case RIGHT:
				new_row.push_back('^');
				break;
			default:
				new_row.push_back('.');
			}
		}
		row.swap(new_row);
		safe_cells += count(begin(row), end(row), '.');
	}

	return safe_cells;
}

TEST_CASE("main")
{
	REQUIRE(38 == Solve(10, ".^^.^.^^^^"));

	const char *PUZZLE = ".^^^.^.^^^^^..^^^..^..^..^^..^.^.^.^^.^^....^.^...^.^^.^^.^^..^^..^.^..^^^.^^...^...^^....^^.^^^^^^^";
	cout << Solve(40, PUZZLE) << endl;
	cout << Solve(400000, PUZZLE) << endl;
}
