#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <boost/icl/interval_set.hpp>
#include "../test.hpp"

namespace {

template <typename T>
void ReadBlacklist(std::istream &is, T &blacklist)
{
	using namespace boost::icl;

	std::string line;
	while (getline(is, line))
	{
		int a{}, b{};
		sscanf(line.c_str(), "%d-%d", &a, &b);
		blacklist += discrete_interval<uint32_t>::closed(a, b);
	}
}

int Solve(std::istream &&is)
{
	using namespace boost::icl;
	interval_set<uint32_t> blacklist;
	ReadBlacklist(is, blacklist);

	auto it = blacklist.begin();
	if (it->lower() > 0)
		return 0;
	return it->upper() + 1;
}

int Solve2(std::istream &&is, uint32_t max)
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

using namespace boost::ut;

suite s = [] {
	"20"_test = [] {
		expect(3_i == Solve(std::istringstream{"5-8\n0-2\n4-7"}));
		Printer::Print(__FILE__, "1", Solve(std::ifstream{INPUT}));

		expect(2_i == Solve2(std::istringstream{"5-8\n0-2\n4-7"}, 9));
		Printer::Print(__FILE__, "2", Solve2(std::ifstream{INPUT}, std::numeric_limits<uint32_t>::max()));
	};
};

} //namespace;
