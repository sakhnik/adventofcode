#include <iostream>
#include <fstream>
#include "../test.hpp"

namespace {

struct Result
{
	Result(unsigned score = 0, unsigned garbage = 0)
		: score(score)
		, garbage(garbage)
	{
	}

	bool operator==(const Result &o) const
	{
		return score == o.score && garbage == o.garbage;
	}

	unsigned score;
	unsigned garbage;
};

std::ostream& operator<<(std::ostream &os, const Result &r)
{
	return os << "(" << r.score << "," << r.garbage << ")";
}

Result Count(const char *s)
{
	Result result;
	unsigned depth{0};

	while (true)
	{
		switch (*s++)
		{
		case 0:
			return result;
		case '{':
			result.score += ++depth;
			break;
		case '}':
			--depth;
			break;
		case '<':
			for (char g = *s++; g != '>'; g = *s++)
			{
				if (g == '!')
					++s;
				else
					++result.garbage;
			}
			break;
		}
	}
}

using namespace boost::ut;

suite s = [] {
	"2017-09"_test = [] {
		expect(eq(Count("{}"), Result(1, 0)));
		expect(eq(Count("{{{}}}"), Result(6, 0)));
		expect(eq(Count("{{},{}}"), Result(5, 0)));
		expect(eq(Count("{{{},{},{{}}}}"), Result(16, 0)));
		expect(eq(Count("{<a>,<a>,<a>,<a>}"), Result(1, 4)));
		expect(eq(Count("{{<ab>},{<ab>},{<ab>},{<ab>}}"), Result(9, 8)));
		expect(eq(Count("{{<!!>},{<!!>},{<!!>},{<!!>}}"), Result(9, 0)));
		expect(Count("{{<a!>},{<a!>},{<a!>},{<ab>}}").score == 3_u);

		std::ifstream ifs(INPUT);
		std::string str((std::istreambuf_iterator<char>(ifs)),
						std::istreambuf_iterator<char>());
		Printer::Print(__FILE__, "1", Count(str.c_str()).score);
		Printer::Print(__FILE__, "2", Count(str.c_str()).garbage);
	};
};

} //namespace;