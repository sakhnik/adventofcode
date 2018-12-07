#include <doctest/doctest.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>

using namespace std;

string Solve(istream &&is, bool use_max)
{
	vector<unordered_map<char, int>> counters;

	string line;
	while (getline(is, line))
	{
		if (counters.empty())
			counters.resize(line.size());
		for (auto i = 0u; i < line.size(); ++i)
			++counters[i][line[i]];
	}

	string ret;
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
	REQUIRE("easter" == Solve(istringstream{test_input}, true));
	cout << Solve(ifstream{INPUT}, true) << endl;

	REQUIRE("advent" == Solve(istringstream{test_input}, false));
	cout << Solve(ifstream{INPUT}, false) << endl;
}
