#include <doctest/doctest.h>
#include <iostream>
#include <bitset>
#include <unordered_map>
#include <queue>
#include <cassert>

using namespace std;

bool IsOpen(int x, int y, int number)
{
	unsigned long long v = x*x + 3*x + 2*x*y + y + y*y + number;
	bitset<64> b(v);
	return (b.count() & 1) == 0;
}

int Solve(int start_x, int start_y, int target_x, int target_y, int number)
{
	assert(IsOpen(start_x, start_y, number));

	unordered_map<size_t, int> distance;
	queue<pair<int,int>> to_search;

	auto idx = [](int x, int y) -> size_t
	{
		size_t i = x;
		i <<= (sizeof(i) * 4);
		return i | y;
	};

	to_search.emplace(start_x, start_y);
	distance[idx(start_x, start_y)] = 0;

	auto search = [&](auto x, auto y, auto dist)
	{
		if (distance.find(idx(x, y)) == distance.end() &&
			IsOpen(x, y, number))
		{
			if (x == target_x && y == target_y)
				throw dist;
			distance[idx(x, y)] = dist;
			to_search.emplace(x, y);
		}
	};

	try
	{
		static bool report2 = false;
		while (true)
		{
			auto xy = to_search.front();
			to_search.pop();
			auto dist = 1 + distance[idx(xy.first, xy.second)];
			if (dist == 51 && !report2)
			{
				// report second result
				cout << distance.size() << endl;
				report2 = true;
			}

			// Try to go left
			if (xy.first > 0)
				search(xy.first - 1, xy.second, dist);

			// Try to go up
			if (xy.second > 0)
				search(xy.first, xy.second - 1, dist);

			search(xy.first + 1, xy.second, dist);
			search(xy.first, xy.second + 1, dist);
		}
	}
	catch (int dist)
	{
		return dist;
	}

	return -1;
}

TEST_CASE(TEST_NAME)
{
	REQUIRE(11 == Solve(1, 1, 7, 4, 10));

	cout << Solve(1, 1, 31, 39, 1350) << endl;
}
