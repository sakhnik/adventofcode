#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include "../test.hpp"
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
	for (const auto &c : counters)
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

using namespace boost::ut;

suite s = [] {
	"06"_test = [] {
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
		using namespace std::string_literals;
		expect(eq("easter"s, Solve(std::istringstream{test_input}, true)));
		Printer::Print(__FILE__, "1", Solve(std::ifstream{INPUT}, true));

		expect(eq("advent"s, Solve(std::istringstream{test_input}, false)));
		Printer::Print(__FILE__, "2", Solve(std::ifstream{INPUT}, false));
	};
};

} //namespace;
