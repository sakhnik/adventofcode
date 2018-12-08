#include <doctest/doctest.h>
#include <iostream>
#include <vector>
#include <cassert>
#include <list>

namespace {

int Solve(int count)
{
	std::vector<int> circle(count);
	for (int i = 0; i != count; ++i)
		circle[i] = i + 1;
	circle.back() = 0;

	int turn = 0;
	while (true)
	{
		int next = circle[circle[turn]];
		if (next == turn)
			return next + 1;
		circle[turn] = next;
		turn = next;
	}
}

int Solve2(int count)
{
	// We'll be having two iterators in the linked list:
	// one pointing to the currently acting gnome,
	// the other pointing to the gnome to steal from (opposite site of the circle)

	std::list<int> circle;
	for (int i = 0; i != count; ++i)
		circle.emplace_back(i + 1);

	auto cur = circle.begin();
	auto op = circle.begin();
	int behind = 0;

	for (; count > 1; --count, ++cur, --behind)
	{
		assert((size_t)count == circle.size());
		for (int n = count / 2; behind != n; ++behind)
		{
			if (++op == circle.end())
				op = circle.begin();
		}

		if (cur == circle.end())
			cur = circle.begin();

		op = circle.erase(op);
		if (op == circle.end())
			op = circle.begin();
	}

	return circle.front();
}

} //namespace;

TEST_CASE(TEST_NAME)
{
	REQUIRE(3 == Solve(5));
	MESSAGE(Solve(3004953));

	REQUIRE(2 == Solve2(5));
	MESSAGE(Solve2(3004953));
}
