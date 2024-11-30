#include <fstream>
#include "../test.hpp"
namespace {

unsigned Sum(std::string_view s, unsigned dist)
{
	unsigned sum{0};

	for (size_t i = 0, n = s.size(); i != n; ++i)
	{
		if (s[i] == s[(i+dist) % s.size()])
			sum += s[i] - '0';
	}

	return sum;
}

using namespace boost::ut;

suite s = [] {
	"01"_test = [] {
		expect(3_u == Sum("1122", 1));
		expect(4_u == Sum("1111", 1));
		expect(0_u == Sum("1234", 1));
		expect(9_u == Sum("91212129", 1));
		expect(6_u == Sum("1212", 2));
		expect(0_u == Sum("1221", 2));
		expect(4_u == Sum("123425", 3));
		expect(12_u == Sum("123123", 3));
		expect(4_u == Sum("12131415", 4));

		std::string input;
		getline(std::ifstream(INPUT), input);
		Printer::Print(__FILE__, "1", Sum(input, 1));
		Printer::Print(__FILE__, "2", Sum(input, input.size() / 2));
	};
};

} //namespace;
