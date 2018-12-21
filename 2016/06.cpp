#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>

namespace {

std::string Solve(std::istream &&is, bool use_max)
{
	std::vector<std::unordered_map<char, int>> counters;

	std::string line;
	while (getline(is, line))
	{
		if (counters.empty())
			counters.resize(line.size());
		for (auto i = 0u; i < line.size(); ++i)
			++counters[i][line[i]];
	}

	std::string ret;
	for (const auto c : counters)
	{
		auto it = use_max
			? max_element(begin(c), end(c),
						  [](auto a, auto b) { return a.second < b.second; })
			: min_element(begin(c), end(c),
						  [](auto a, auto b) { return a.second < b.second; })
			;
		ret.push_back(it->first);
	}
	return ret;
}

} //namespace;

TEST_CASE(TEST_NAME)
{
	const char *const test_input =
		R"(eedadn
drvtee
eandsr
raavrd
atevrs
tsrnev
sdttsa
rasrtv
nssdts
ntnada
svetve
tesnvt
vntsnd
vrdear
dvrsen
enarar)";
	REQUIRE("easter" == Solve(std::istringstream{test_input}, true));
	MESSAGE(Solve(std::ifstream{INPUT}, true));

	REQUIRE("advent" == Solve(std::istringstream{test_input}, false));
	MESSAGE(Solve(std::ifstream{INPUT}, false));
}
