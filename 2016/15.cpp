#include <cassert>
#include <vector>
#include "../test.hpp"

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

using namespace boost::ut;

suite s = [] {
	"2016-15"_test = [] {
		expect(5_i == Solve({{5, 4}, {2, 1}}));
		std::vector<Disk> disks = {
			{13, 1},
			{19, 10},
			{3, 2},
			{7, 1},
			{5, 3},
			{17, 5}};
		Printer::Print(__FILE__, "1", Solve(disks));

		disks.push_back({11, 0});
		Printer::Print(__FILE__, "2", Solve(disks));
	};
};

} //namespace;