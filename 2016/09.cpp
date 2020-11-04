#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <cstring>
#include "../test.hpp"

namespace {

std::string Decompress(const std::string &s)
{
	std::string ret;

	for (size_t i = 0; i != s.size(); )
	{
		auto ch = s[i];

		if (ch == '(')
		{
			int span{0}, count{0};
			int fields = sscanf(&s[i], "(%dx%d)", &span, &count);
			if (fields != 2)
				assert(false);
			i = s.find(')', i);
			assert(i != s.npos);
			++i;

			for (int j = 0; j < count; ++j)
			{
				ret.append(s, i, span);
			}

			i += span;
		}
		else
		{
			ret.push_back(ch);
			++i;
		}
	}

	return ret;
}

size_t Count(const char *s, const char *send);

std::pair<size_t, const char *> CountPattern(const char *s)
{
	int span{0}, count{0};
	int fields = sscanf(s, "(%dx%d)", &span, &count);
	using namespace boost::ut;
	expect(fields == 2_i);

	const char *rbr = strchr(s, ')');
	expect(rbr);
	++rbr;

	const char *send = rbr + span;

	return { size_t(count) * Count(rbr, send), send };
}

size_t Count(const char *s, const char *send)
{
	size_t count{0};

	while (s != send)
	{
		if (*s == '(')
		{
			auto res = CountPattern(s);
			count += res.first;
			s = res.second;
		}
		else
		{
			if (!isspace(*s))
				++count;
			++s;
		}
	}

	return count;
}

size_t Version2(const std::string &s)
{
	return Count(s.data(), s.data() + s.size());
}

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
	"2016-09"_test = [] {
		expect(eq(Decompress("ADVENT"), "ADVENT"s));
		expect(eq(Decompress("A(1x5)BC"), "ABBBBBC"s));
		expect(eq(Decompress("(3x3)XYZ"), "XYZXYZXYZ"s));
		expect(eq(Decompress("A(2x2)BCD(2x2)EFG"), "ABCBCDEFEFG"s));
		expect(eq(Decompress("(6x1)(1x3)A"), "(1x3)A"s));
		expect(eq(Decompress("X(8x2)(3x3)ABCY"), "X(3x3)ABC(3x3)ABCY"s));

		std::ifstream ifs(INPUT);
		std::string s;
		getline(ifs, s, char{0});

		auto d = Decompress(s);
		Printer::Print(__FILE__, "1", std::count_if(begin(d), end(d), [](char c) { return !isspace(c); }));

		expect(9_u == Version2("(3x3)XYZ"));
		expect(20_u == Version2("X(8x2)(3x3)ABCY"));
		expect(241920_u == Version2("(27x12)(20x12)(13x14)(7x10)(1x12)A"));
		expect(445_u == Version2("(25x3)(3x3)ABC(2x3)XY(5x2)PQRSTX(18x9)(3x2)TWO(5x7)SEVEN"));

		Printer::Print(__FILE__, "2", Version2(s));
	};
};

} //namespace;
