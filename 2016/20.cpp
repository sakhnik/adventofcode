#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <boost/icl/interval_set.hpp>

using namespace std;

namespace {

template <typename T>
void ReadBlacklist(istream &is, T &blacklist)
{
	using namespace boost::icl;

	string line;
	while (getline(is, line))
	{
		int a{}, b{};
		sscanf(line.c_str(), "%d-%d", &a, &b);
		blacklist += discrete_interval<uint32_t>::closed(a, b);
	}
}

int Solve(istream &&is)
{
	using namespace boost::icl;
	interval_set<uint32_t> blacklist;
	ReadBlacklist(is, blacklist);

	auto it = blacklist.begin();
	if (it->lower() > 0)
		return 0;
	return it->upper() + 1;
}

int Solve2(istream &&is, uint32_t max)
{
	using namespace boost::icl;
	interval_set<uint32_t> blacklist;
	ReadBlacklist(is, blacklist);

	uint32_t sum{};
	uint32_t cur{};

	for (const auto i : blacklist)
	{
		sum += i.lower() - cur;
		cur = i.upper() + 1;
	}

	sum += max - cur + 1;
	return sum;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
	REQUIRE(3 == Solve(istringstream("5-8\n0-2\n4-7")));
	cout << Solve(ifstream(INPUT)) << endl;

	REQUIRE(2 == Solve2(istringstream("5-8\n0-2\n4-7"), 9));
	cout << Solve2(ifstream(INPUT), numeric_limits<uint32_t>::max()) << endl;
}
