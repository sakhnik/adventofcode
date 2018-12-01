#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <cstring>

using namespace std;

string Decompress(const std::string &s)
{
	string ret;

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

pair<size_t, const char *> CountPattern(const char *s)
{
	int span{0}, count{0};
	int fields = sscanf(s, "(%dx%d)", &span, &count);
	assert(fields == 2);

	const char *rbr = strchr(s, ')');
	assert(rbr);
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

size_t Version2(const string &s)
{
	return Count(s.data(), s.data() + s.size());
}

TEST_CASE("main")
{
	REQUIRE(Decompress("ADVENT") == "ADVENT");
	REQUIRE(Decompress("A(1x5)BC") == "ABBBBBC");
	REQUIRE(Decompress("(3x3)XYZ") == "XYZXYZXYZ");
	REQUIRE(Decompress("A(2x2)BCD(2x2)EFG") == "ABCBCDEFEFG");
	REQUIRE(Decompress("(6x1)(1x3)A") == "(1x3)A");
	REQUIRE(Decompress("X(8x2)(3x3)ABCY") == "X(3x3)ABC(3x3)ABCY");

	ifstream ifs(INPUT);
	string s;
	getline(ifs, s, char{-1});

	auto d = Decompress(s);
	cout << count_if(begin(d), end(d), [](char c) { return !isspace(c); }) << endl;


	REQUIRE(Version2("(3x3)XYZ") == 9);
	REQUIRE(Version2("X(8x2)(3x3)ABCY") == 20);
	REQUIRE(Version2("(27x12)(20x12)(13x14)(7x10)(1x12)A") == 241920);
	REQUIRE(Version2("(25x3)(3x3)ABC(2x3)XY(5x2)PQRSTX(18x9)(3x2)TWO(5x7)SEVEN") == 445);

	cout << Version2(s) << endl;
}
