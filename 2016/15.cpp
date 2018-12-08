#include <doctest/doctest.h>
#include <iostream>
#include <cassert>
#include <vector>

namespace {

struct Disk
{
	int T; // period
	int phi; // initial state
};

int Solve(const std::vector<Disk> &disks)
{
	for (int n = 0; ; ++n)
	{
		bool ok = true;
		for (unsigned i = 0; i != disks.size(); ++i)
		{
			if ((n + i + 1 + disks[i].phi) % disks[i].T)
			{
				ok = false;
				break;
			}
		}

		if (ok)
			return n;
	}
}

} //namespace;

TEST_CASE(TEST_NAME)
{
	REQUIRE(5 == Solve({{5, 4}, {2, 1}}));
	std::vector<Disk> disks = {
		{ 13, 1 },
		{ 19, 10 },
		{ 3, 2 },
		{ 7, 1 },
		{ 5, 3 },
		{ 17, 5 }
	};
	MESSAGE(Solve(disks));

	disks.push_back({11, 0});
	MESSAGE(Solve(disks));
}
